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

#include <fstream>
#include <iostream>
#include <numeric>
#include <string>
#include <vector>

/* Defines! */
// These benchmarks should best be done 1 (category) at a time to avoid trashing of the cache

#define ENABLE_CUSTOMECS_BENCHMARKS
#define BENCHMARK_CUSTOMECS_CREATION
#define BENCHMARK_CUSTOMECS_UPDATE

//#define ENABLE_GAMEOBJECT_BENCHMARKS
//#define BENCHMARK_GAMEOBJECT_CREATION
//#define BENCHMARK_GAMEOBJECT_UPDATE
//
//#define ENABLE_ENTT_BENCHMARKS
//#define BENCHMARK_ENTT_CREATION
//#define BENCHMARK_ENTT_UPDATE

// #define WRITE_TIMES_TO_CSV_FILES

namespace
{
	__forceinline static double GetAverage(const std::vector<double>& arr)
	{
		return std::accumulate(arr.cbegin(), arr.cend(), 0.0) / static_cast<double>(arr.size());
	}

#ifdef WRITE_TIMES_TO_CSV_FILES

	static void WriteTimesToCSVFile(const std::string& file, const std::vector<double>& times)
	{
		std::fstream stream{ file, std::ios::out | std::ios::trunc };

		for (size_t i{}; i < times.size() - 1; ++i)
			stream << times[i] << ",";

		stream << times.back();

		stream.close(); // technically not necessary, but eh
	}

#endif // WRITE_TIMES_TO_CSV_FILES

}

int RunBenchmarks()
{
	using namespace ECS;
	using namespace GO;

	/* Benchmarking Constants */
	constexpr int Iterations{ 100 };
	constexpr Entity AmountOfEntities{ 1'000 };

#ifdef ENABLE_CUSTOMECS_BENCHMARKS

#ifdef BENCHMARK_CUSTOMECS_CREATION

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

#endif // BENCHMARK_CUSTOMECS_CREATION

#ifdef BENCHMARK_CUSTOMECS_UPDATE

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

#endif // BENCHMARK_CUSTOMECS_UPDATE

#endif // ENABLE_CUSTOMECS_BENCHMARKS


#ifdef ENABLE_GAMEOBJECT_BENCHMARKS

#ifdef BENCHMARK_GAMEOBJECT_CREATION

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

#endif // BENCHMARK_GAMEOBJECT_CREATION

#ifdef BENCHMARK_GAMEOBJECT_UPDATE

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

#endif // BENCHMARK_GAMEOBJECT_UPDATE

#endif // ENABLE_GAMEOBJECT_BENCHMARKS


#ifdef ENABLE_ENTT_BENCHMARKS

#ifdef BENCHMARK_ENTT_CREATION

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

#endif // BENCHMARK_ENTT_CREATION

#ifdef BENCHMARK_ENTT_UPDATE

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

#endif // BENCHMARK_ENTT_UPDATE

#endif // ENABLE_ENTT_BENCHMARKS

	/* Print results */
	std::cout << "Amount of entities: " << AmountOfEntities << "\n";
	std::cout << "Iterations: " << Iterations << "\n\n";

#ifdef ENABLE_CUSTOMECS_BENCHMARKS

#ifdef BENCHMARK_CUSTOMECS_CREATION

	std::cout << "ECS Init Average:\t\t" << GetAverage(ecsInitTimes) << " milliseconds\n";

#ifdef WRITE_TIMES_TO_CSV_FILES

	WriteTimesToCSVFile(R"(.\Benchmarks/CE_CREATION_BM.csv)", ecsInitTimes);

#endif // WRITE_TIMES_TO_CSV_FILES

#endif // BENCHMARK_CUSTOMECS_CREATION

#ifdef BENCHMARK_CUSTOMECS_UPDATE

	std::cout << "ECS Update Average:\t\t" << GetAverage(ecsUpdateTimes) << " milliseconds\n\n";

#ifdef WRITE_TIMES_TO_CSV_FILES

	WriteTimesToCSVFile(R"(.\Benchmarks\CE_UPDATE_BM.csv)", ecsUpdateTimes);

#endif // WRITE_TIMES_TO_CSV_FILES

#endif // BENCHMARK_CUSTOMECS_UPDATE

#endif // ENABLE_CUSTOMECS_BENCHMARKS


#ifdef ENABLE_GAMEOBJECT_BENCHMARKS

#ifdef BENCHMARK_GAMEOBJECT_CREATION

	std::cout << "GO Init Average:\t\t" << GetAverage(goInitTimes) << " milliseconds\n";

#ifdef WRITE_TIMES_TO_CSV_FILES

	WriteTimesToCSVFile(R"(Benchmarks/GO_CREATION_BM.csv)", goInitTimes);

#endif // WRITE_TIMES_TO_CSV_FILES

#endif // BENCHMARK_GAMEOBJECT_CREATION

#ifdef BENCHMARK_GAMEOBJECT_UPDATE

	std::cout << "GO Update Average:\t\t" << GetAverage(goUpdateTimes) << " milliseconds\n\n";

#ifdef WRITE_TIMES_TO_CSV_FILES

	WriteTimesToCSVFile(R"(Benchmarks/GO_UPDATE_BM.csv)", goUpdateTimes);

#endif // WRITE_TIMES_TO_CSV_FILES

#endif // BENCHMARK_GAMEOBJECT_UPDATE

#endif // ENABLE_GAMEOBJECT_BENCHMARKS


#ifdef ENABLE_ENTT_BENCHMARKS

#ifdef BENCHMARK_ENTT_CREATION

	std::cout << "ENTT Init Average:\t\t" << GetAverage(enttInitTimes) << " milliseconds\n";

#ifdef WRITE_TIMES_TO_CSV_FILES

	WriteTimesToCSVFile(R"(Benchmarks/ET_CREATION_BM.csv)", enttInitTimes);

#endif // WRITE_TIMES_TO_CSV_FILES

#endif // BENCHMARK_ENTT_CREATION

#ifdef BENCHMARK_ENTT_UPDATE

	std::cout << "ENTT Update Average:\t\t" << GetAverage(enttUpdateTimes) << " milliseconds\n\n";

#ifdef WRITE_TIMES_TO_CSV_FILES

	WriteTimesToCSVFile(R"(Benchmarks/ET_UPDATE_BM.csv)", enttUpdateTimes);

#endif // WRITE_TIMES_TO_CSV_FILES

#endif // BENCHMARK_ENTT_UPDATE

#endif // ENABLE_ENTT_BENCHMARKS

	return 0;
}