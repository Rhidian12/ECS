#include "ECSConstants.h"

#include "Registry/Registry.h"

#include "GOComponent/GOComponent.h"
#include "GameObject/GameObject.h"
#include "Timer/Timer.h"

#include "entt/entt.hpp"

#include "ENTTComponents/ENTTComponents.h"
#include "ECSComponents/ECSComponents.h"
#include "GOComponents/GOComponents.h"

#include "Benchmark/BenchmarkUtils.h"

#include <iostream>
#include <numeric>
#include <vector>
#include <string>
#include <fstream>
#include <vld.h>

__forceinline static double GetAverage(const std::vector<double>& arr)
{
	return std::accumulate(arr.cbegin(), arr.cend(), 0.0) / static_cast<double>(arr.size());
}

static void WriteTimesToCSVFile(const std::string& file, const std::vector<double>& times)
{
	std::fstream stream{ file, std::ios::out | std::ios::trunc };

	for (size_t i{}; i < times.size() - 1; ++i)
		stream << times[i] << ",";

	stream << times.back();

	stream.close();
}

/* Defines! */
// #define CUSTOMECS
// #define CUSTOMECS_CREATION
// #define CUSTOMECS_UPDATE

// #define GAMEOBJECT
// #define GAMEOBJECT_CREATION
// #define GAMEOBJECT_UPDATE

#define ENTT
#define ENTT_CREATION
#define ENTT_UPDATE

#define WRITE_TO_FILE

// #define UNIT_TESTS
#define BENCHMARKS

#ifdef BENCHMARKS

int main(int*, char* [])
{
	using namespace ECS;
	using namespace GO;

	/* Benchmarking Constants */
	constexpr int Iterations{ 100 };
	constexpr Entity AmountOfEntities{ 1'000'000 };

#ifdef CUSTOMECS

#ifdef CUSTOMECS_CREATION

	std::vector<double> ecsInitTimes{};

	{
		Benchmark::BenchmarkUtils benchmarker{};
		ECS::Registry registry{};

		benchmarker.SetOnFunctionStart([&registry]()->void
			{
				registry.Clear();
			});

		ecsInitTimes = benchmarker.BenchmarkFunction(Iterations, [&registry, AmountOfEntities]()->void
				{
					for (size_t i{}; i < AmountOfEntities; ++i)
					{
						Entity entity{ registry.CreateEntity() };

						registry.AddComponent<TransformComponent>(entity);
						registry.AddComponent<RigidBodyComponent>(entity);
						registry.AddComponent<GravityComponent>(entity);
					}
				});
	}

#endif // CUSTOMECS_CREATION

#ifdef CUSTOMECS_UPDATE

	std::vector<double> ecsUpdateTimes{};

	{
		Benchmark::BenchmarkUtils benchmarker{};
		ECS::Registry ecsRegistry{};

		for (size_t i{}; i < AmountOfEntities; ++i)
		{
			Entity entity{ ecsRegistry.CreateEntity() };

			ecsRegistry.AddComponent<TransformComponent>(entity);
			ecsRegistry.AddComponent<RigidBodyComponent>(entity);
			ecsRegistry.AddComponent<GravityComponent>(entity);
		}

		ecsUpdateTimes = benchmarker.BenchmarkFunction(Iterations, [&ecsRegistry]()->void
			{
				auto gravityView = ecsRegistry.CreateView<GravityComponent, RigidBodyComponent>();

				gravityView.ForEach([](const auto& gravity, auto& rigidBody)->void
					{
						rigidBody.Velocity.y += gravity.Gravity * rigidBody.Mass;
					});

				auto physicsView = ecsRegistry.CreateView<RigidBodyComponent, TransformComponent>();

				physicsView.ForEach([](const auto& rigidBody, auto& transform)->void
					{
						transform.Position.x += rigidBody.Velocity.x;
						transform.Position.y += rigidBody.Velocity.y;
					});
			});
	}

#endif // CUSTOMECS_UPDATE

#endif // CUSTOMECS

#ifdef GAMEOBJECT

#ifdef GAMEOBJECT_CREATION

	std::vector<double> goInitTimes{};

	{
		Benchmark::BenchmarkUtils benchmarker{};
		std::vector<GO::GameObject*> gameObjects{};

		benchmarker.SetOnFunctionStart([&gameObjects]()->void
			{
				for (auto* pG : gameObjects)
					delete pG;

				gameObjects.clear();
			});

		goInitTimes = benchmarker.BenchmarkFunction(Iterations, [&gameObjects, AmountOfEntities]()->void
			{
				for (size_t i{}; i < AmountOfEntities; ++i)
				{
					GameObject* pG{ new GameObject{} };

					pG->AddComponent(new GOGravityComponent{});
					pG->AddComponent(new GORigidBodyComponent{ pG->GetComponent<GOGravityComponent>() });
					pG->AddComponent(new GOTransformComponent{ pG->GetComponent<GORigidBodyComponent>() });

					gameObjects.push_back(pG);
				}
			});
	}

#endif // GAMEOBJECT_CREATION

#ifdef GAMEOBJECT_UPDATE

	std::vector<double> goUpdateTimes{};

	{
		Benchmark::BenchmarkUtils benchmarker{};
		std::vector<GO::GameObject*> gameObjects{};

		for (size_t i{}; i < AmountOfEntities; ++i)
		{
			GameObject* pG{ new GameObject{} };

			pG->AddComponent(new GOGravityComponent{});
			pG->AddComponent(new GORigidBodyComponent{ pG->GetComponent<GOGravityComponent>() });
			pG->AddComponent(new GOTransformComponent{ pG->GetComponent<GORigidBodyComponent>() });

			gameObjects.push_back(pG);
		}

		goUpdateTimes = benchmarker.BenchmarkFunction(Iterations, [&gameObjects]()->void
			{
				for (GameObject* pG : gameObjects)
					pG->Update();
			});
	}

#endif // GAMEOBJECT_UPDATE

#endif // GAMEOBJECT

#ifdef ENTT

#ifdef ENTT_CREATION

	std::vector<double> enttInitTimes{};

	{
		Benchmark::BenchmarkUtils benchmarker{};
		entt::registry registry{};

		benchmarker.SetOnFunctionStart([&registry]()->void
			{
				registry.clear();
			});

		enttInitTimes = benchmarker.BenchmarkFunction(Iterations, [&registry, AmountOfEntities]()->void
			{
				for (size_t i{}; i < AmountOfEntities; ++i)
				{
					auto enttEntity{ registry.create() };

					registry.emplace<ENTTGravity>(enttEntity);
					registry.emplace<ENTTTransformComponent>(enttEntity);
					registry.emplace<ENTTRigidBodyComponent>(enttEntity);
				}
			});
	}

#endif // ENTT_CREATION

#ifdef ENTT_UPDATE

	std::vector<double> enttUpdateTimes{};

	{
		Benchmark::BenchmarkUtils benchmarker{};
		entt::registry registry{};

		for (size_t i{}; i < AmountOfEntities; ++i)
		{
			auto enttEntity{ registry.create() };

			registry.emplace<ENTTGravity>(enttEntity);
			registry.emplace<ENTTTransformComponent>(enttEntity);
			registry.emplace<ENTTRigidBodyComponent>(enttEntity);
		}

		enttUpdateTimes = benchmarker.BenchmarkFunction(Iterations, [&registry]()->void
			{
				auto gravityView = registry.view<const ENTTGravity, ENTTRigidBodyComponent>();

				gravityView.each([](const auto& gravity, auto& rigidBody)
					{
						rigidBody.Velocity.y += gravity.Gravity * rigidBody.Mass;
					});

				auto physicsView = registry.view<const ENTTRigidBodyComponent, ENTTTransformComponent>();

				physicsView.each([](const auto& rigidBody, auto& transform)
					{
						transform.Position.x += rigidBody.Velocity.x;
						transform.Position.y += rigidBody.Velocity.y;
					});
			});
	}

#endif // ENTT_UPDATE

#endif // ENTT

	/* Print results */
	std::cout << "Amount of entities: " << AmountOfEntities << "\n";
	std::cout << "Iterations: " << Iterations << "\n\n";

#ifdef CUSTOMECS

#ifdef CUSTOMECS_CREATION

	std::cout << "ECS Init Average:\t\t" << GetAverage(ecsInitTimes) << " milliseconds\n";

#ifdef WRITE_TO_FILE

	WriteTimesToCSVFile(R"(.\Benchmarks/CE_CREATION_BM.csv)", ecsInitTimes);

#endif

#endif // CUSTOMECS_CREATION

#ifdef CUSTOMECS_UPDATE

	std::cout << "ECS Update Average:\t\t" << GetAverage(ecsUpdateTimes) << " milliseconds\n\n";

#ifdef WRITE_TO_FILE

	WriteTimesToCSVFile(R"(.\Benchmarks\CE_UPDATE_BM.csv)", ecsUpdateTimes);

#endif

#endif // CUSTOMECS_UPDATE

#endif // CUSTOMECS

#ifdef GAMEOBJECT

#ifdef GAMEOBJECT_CREATION

	std::cout << "GO Init Average:\t\t" << GetAverage(goInitTimes) << " milliseconds\n";

#ifdef WRITE_TO_FILE

	WriteTimesToCSVFile(R"(Benchmarks/GO_CREATION_BM.csv)", goInitTimes);

#endif

#endif // GAMEOBJECT_CREATION

#ifdef GAMEOBJECT_UPDATE

	std::cout << "GO Update Average:\t\t" << GetAverage(goUpdateTimes) << " milliseconds\n\n";

#ifdef WRITE_TO_FILE

	WriteTimesToCSVFile(R"(Benchmarks/GO_UPDATE_BM.csv)", goUpdateTimes);

#endif

#endif // GAMEOBJECT_UPDATE

#endif // GAMEOBJECT

#ifdef ENTT

#ifdef ENTT_CREATION

	std::cout << "ENTT Init Average:\t\t" << GetAverage(enttInitTimes) << " milliseconds\n";

#ifdef WRITE_TO_FILE

	WriteTimesToCSVFile(R"(Benchmarks/ET_CREATION_BM.csv)", enttInitTimes);

#endif

#endif // ENTT_CREATION

#ifdef ENTT_UPDATE

	std::cout << "ENTT Update Average:\t\t" << GetAverage(enttUpdateTimes) << " milliseconds\n\n";

#ifdef WRITE_TO_FILE

	WriteTimesToCSVFile(R"(Benchmarks/ET_UPDATE_BM.csv)", enttUpdateTimes);

#endif

#endif // ENTT_UPDATE

#endif // ENTT

	return 0;
}

#endif

#ifdef UNIT_TESTS
#define CATCH_CONFIG_MAIN
#include "catch.hpp"

TEST_CASE("Testing SparseSet")
{
	ECS::SparseSet<int> set{};

	REQUIRE(set.Size() == 0);

	SECTION("Testing basic adding")
	{
		set.Add(5);

		REQUIRE(set.Contains(5));
		REQUIRE(set.Size() == 1);
		REQUIRE(set.GetIndex(5) == 0); /* should be the first element */
	}

	SECTION("Add and Remove 1 value")
	{
		set.Add(5);

		REQUIRE(set.Contains(5));
		REQUIRE(set.Size() == 1);
		REQUIRE(set.GetIndex(5) == 0); /* should be the first element */

		set.Remove(5);

		REQUIRE(set.Size() == 0);
	}

	SECTION("Testing clear")
	{
		for (int i{}; i < 10; ++i)
		{
			set.Add(i);
		}

		REQUIRE(set.Size() == 10);

		set.Clear();

		REQUIRE(set.Size() == 0);
	}

	SECTION("Testing iterator")
	{
		for (int i{}; i < 10; ++i)
		{
			set.Add(i);
		}

		REQUIRE(set.Size() == 10);

		int counter{};
		for (const int i : set)
		{
			REQUIRE(i == counter++);
		}
	}

	SECTION("Testing Swap")
	{
		for (int i{}; i < 10; ++i)
		{
			set.Add(i);
		}

		REQUIRE(set[0] == 0);
		REQUIRE(set[9] == 9);

		set.Swap(0, 9);

		REQUIRE(set[0] == 9);
		REQUIRE(set[9] == 0);

		set.Remove(0);
		REQUIRE(!set.Contains(0));
	}
}

TEST_CASE("Testing custom ECS")
{
	ECS::Registry registry{};

	REQUIRE(registry.GetAmountOfEntities() == 0);

	SECTION("Making one entity")
	{
		ECS::Entity entity{ registry.CreateEntity() };

		REQUIRE(entity == 0);
	}

	SECTION("Making 10 entities and testing their updates")
	{
		constexpr int size{ 10 };
		float startPositions[size]{};

		for (int i{}; i < size; ++i)
		{
			ECS::Entity entity{ registry.CreateEntity() };

			registry.AddComponent<GravityComponent>(entity);
			registry.AddComponent<TransformComponent>(entity);
			registry.AddComponent<RigidBodyComponent>(entity);

			startPositions[i] = registry.GetComponent<TransformComponent>(entity).Position.y;
		}

		auto view = registry.CreateView<GravityComponent, TransformComponent>();

		view.ForEach([](const auto& grav, auto& trans)
			{
				trans.Position.y += grav.Gravity; // - 981.0
			});

		for (int i{}; i < size; ++i)
		{
			REQUIRE(startPositions[i] > registry.GetComponent<TransformComponent>(i).Position.y);
		}
	}

	SECTION("Removing Entities")
	{
		struct RemoveEntityTestData
		{
			std::string Name;
		};

		for (int i{}; i < 5; ++i)
		{
			ECS::Entity entity{ registry.CreateEntity() };

			registry.AddComponent<RemoveEntityTestData>(entity, std::to_string(i));
		}

		registry.ReleaseEntity(3);

		REQUIRE(registry.GetAmountOfEntities() == 4);
		REQUIRE(registry.GetComponent<RemoveEntityTestData>(2).Name == "2");
		REQUIRE(registry.GetComponent<RemoveEntityTestData>(4).Name == "4");
}
}
#endif