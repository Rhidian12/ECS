#include "ECSConstants.h"

#include "Registry/Registry.h"

#include "GOComponent/GOComponent.h"
#include "GameObject/GameObject.h"
#include "Timer/Timer.h"

#include <iostream>
#include <chrono>
#include <deque>
#include <algorithm>
#include <numeric>
#include <vector>
#include <string>
#include <fstream>
#include <vld.h>

#include "entt/entt.hpp"

#include "ENTTComponents/ENTTComponents.h"
#include "ECSComponents/ECSComponents.h"
#include "GOComponents/GOComponents.h"

template<typename Fn>
double Benchmark(Fn&& fn)
{
	using namespace ECS::Time;

	Timepoint t1{ Timer::Now() };

	fn();

	Timepoint t2{ Timer::Now() };

	return (t2 - t1).Count<TimeLength::MilliSeconds>();
}

double GetAverage(const std::deque<double>& arr)
{
	return std::accumulate(arr.cbegin(), arr.cend(), 0.0) / static_cast<double>(arr.size());
}

#pragma region Test Update Functions
void ENTTGravityUpdate(entt::registry& registry)
{
	auto view = registry.view<const ENTTGravity, ENTTRigidBodyComponent>();

	view.each([](const auto& gravity, auto& rigidBody)
		{
			rigidBody.Velocity.y += gravity.Gravity * rigidBody.Mass;
		});
}
void ENTTPhysicsUpdate(entt::registry& registry)
{
	auto view = registry.view<const ENTTRigidBodyComponent, ENTTTransformComponent>();

	view.each([](const auto& rigidBody, auto& transform)
		{
			transform.Position.x += rigidBody.Velocity.x;
			transform.Position.y += rigidBody.Velocity.y;
		});
}

void GravityUpdate(ECS::Registry& registry)
{
	auto view = registry.CreateView<GravityComponent, RigidBodyComponent>();

	view.ForEach([](const auto& gravity, auto& rigidBody)->void
		{
			rigidBody.Velocity.y += gravity.Gravity * rigidBody.Mass;
		});
}
void PhysicsUpdate(ECS::Registry& registry)
{
	auto view = registry.CreateView<RigidBodyComponent, TransformComponent>();

	view.ForEach([](const auto& rigidBody, auto& transform)->void
		{
			transform.Position.x += rigidBody.Velocity.x;
			transform.Position.y += rigidBody.Velocity.y;
		});
}
#pragma endregion

/* Defines! */
#define CUSTOMECS
#define CUSTOMECS_CREATION
#define CUSTOMECS_UPDATE

// #define GAMEOBJECT
// #define GAMEOBJECT_CREATION
// #define GAMEOBJECT_UPDATE
// 
// #define ENTT
// #define ENTT_CREATION
// #define ENTT_UPDATE

#define UNIT_TESTS
// #define BENCHMARKS

#ifdef BENCHMARKS
int main(int*, char* [])
{
	using namespace ECS;
	using namespace GO;

	/* Benchmarking Constants */
	constexpr int Iterations{ 1 };
	constexpr Entity AmountOfEntities{ 10 };

#ifdef CUSTOMECS

#ifdef CUSTOMECS_CREATION
	std::deque<double> ecsInitTimes{};
#endif // CUSTOMECS_CREATION

#ifdef CUSTOMECS_UPDATE
	std::deque<double> ecsUpdateTimes{};
#endif // CUSTOMECS_UPDATE

	{
	#ifdef CUSTOMECS_UPDATE
		ECS::Registry ecsRegistry{};

		for (size_t i{}; i < AmountOfEntities; ++i)
		{
			Entity entity{ ecsRegistry.CreateEntity() };

			ecsRegistry.AddComponent<TransformComponent>(entity);
			ecsRegistry.AddComponent<RigidBodyComponent>(entity);
			ecsRegistry.AddComponent<GravityComponent>(entity);
		}
	#endif // CUSTOMECS_UPDATE

		for (int i{}; i < Iterations; ++i)
		{
		#ifdef CUSTOMECS_CREATION
			ecsInitTimes.push_back(Benchmark([AmountOfEntities]()->void
				{
					ECS::Registry registry{};

					for (size_t i{}; i < AmountOfEntities; ++i)
					{
						Entity entity{ registry.CreateEntity() };

						registry.AddComponent<TransformComponent>(entity);
						registry.AddComponent<RigidBodyComponent>(entity);
						registry.AddComponent<GravityComponent>(entity);
					}
				}));
		#endif // CUSTOMECS_CREATION

		#ifdef CUSTOMECS_UPDATE
			ecsUpdateTimes.push_back(Benchmark([&ecsRegistry]()->void
				{
					GravityUpdate(ecsRegistry);
					PhysicsUpdate(ecsRegistry);
				}));
		#endif // CUSTOMECS_UPDATE
		}
	}
#endif // CUSTOMECS

#ifdef GAMEOBJECT

#ifdef GAMEOBJECT_CREATION
	std::deque<double> goInitTimes{};
#endif // GAMEOBJECT_CREATION

#ifdef GAMEOBJECT_UPDATE
	std::deque<double> goUpdateTimes{};
#endif // GAMEOBJECT_UPDATE

	{
	#ifdef GAMEOBJECT_UPDATE
		std::vector<GO::GameObject*> gameObjects{};

		for (size_t i{}; i < AmountOfEntities; ++i)
		{
			GameObject* pG{ new GameObject{} };

			pG->AddComponent(new GOGravityComponent{});
			pG->AddComponent(new GORigidBodyComponent{ pG->GetComponent<GOGravityComponent>() });
			pG->AddComponent(new GOTransformComponent{ pG->GetComponent<GORigidBodyComponent>() });

			gameObjects.push_back(pG);
		}
	#endif // GAMEOBJECT_UPDATE

		for (int i{}; i < Iterations; ++i)
		{
		#ifdef GAMEOBJECT_CREATION
			goInitTimes.push_back(Benchmark([AmountOfEntities]()->void
				{
					std::vector<GameObject*> initGameobjects{};

					for (size_t i{}; i < AmountOfEntities; ++i)
					{
						GameObject* pG{ new GameObject{} };

						pG->AddComponent(new GOGravityComponent{});
						pG->AddComponent(new GORigidBodyComponent{ pG->GetComponent<GOGravityComponent>() });
						pG->AddComponent(new GOTransformComponent{ pG->GetComponent<GORigidBodyComponent>() });

						initGameobjects.push_back(pG);
					}

					for (GameObject* pG : initGameobjects)
						delete pG;
				}));
		#endif // GAMEOBJECT_CREATION

		#ifdef GAMEOBJECT_UPDATE
			goUpdateTimes.push_back(Benchmark([&gameObjects]()
				{
					for (GameObject* const pG : gameObjects)
					{
						pG->Update();
					}
				}));
		#endif // GAMEOBJECT_UPDATE
		}

	#ifdef GAMEOBJECT_UPDATE
		for (GameObject* pG : gameObjects)
			delete pG;
	#endif // GAMEOBJECT_UPDATE
	}
#endif

#ifdef ENTT

#ifdef ENTT_CREATION
	std::deque<double> enttInitTimes{};
#endif // ENTT_CREATION

#ifdef ENTT_UPDATE
	std::deque<double> enttUpdateTimes{};
#endif // ENTT_UPDATE

	{
	#ifdef ENTT_UPDATE
		entt::registry entt{};
		for (size_t i{}; i < AmountOfEntities; ++i)
		{
			auto enttEntity{ entt.create() };
			entt.emplace<ENTTGravity>(enttEntity);
			entt.emplace<ENTTTransformComponent>(enttEntity);
			entt.emplace<ENTTRigidBodyComponent>(enttEntity);
		}
	#endif // ENTT_UPDATE

		for (int i{}; i < Iterations; ++i)
		{
		#ifdef ENTT_CREATION
			enttInitTimes.push_back(Benchmark([AmountOfEntities]()->void
				{
					entt::registry initRegistry{};

					for (size_t i{}; i < AmountOfEntities; ++i)
					{
						auto enttEntity{ initRegistry.create() };
						initRegistry.emplace<ENTTGravity>(enttEntity);
						initRegistry.emplace<ENTTTransformComponent>(enttEntity);
						initRegistry.emplace<ENTTRigidBodyComponent>(enttEntity);
					}
				}));
		#endif // ENTT_CREATION

		#ifdef ENTT_UPDATE
			enttUpdateTimes.push_back(Benchmark([&entt]()
				{
					ENTTGravityUpdate(entt);
					ENTTPhysicsUpdate(entt);
				}));
		#endif // ENTT_UPDATE
		}
	}
#endif

	/* Print results */
	std::cout << "Amount of entities: " << AmountOfEntities << "\n";
	std::cout << "Iterations: " << Iterations << "\n\n";

#ifdef CUSTOMECS

#ifdef CUSTOMECS_CREATION
	std::cout << "ECS Init Average:\t\t" << GetAverage(ecsInitTimes) << " milliseconds\n";
#endif // CUSTOMECS_CREATION

#ifdef CUSTOMECS_UPDATE
	std::cout << "ECS Update Average:\t\t" << GetAverage(ecsUpdateTimes) << " milliseconds\n\n";
#endif // CUSTOMECS_UPDATE

#endif // CUSTOMECS

#ifdef GAMEOBJECT

#ifdef GAMEOBJECT_CREATION
	std::cout << "GO Init Average:\t\t" << GetAverage(goInitTimes) << " milliseconds\n";
#endif // GAMEOBJECT_CREATION

#ifdef GAMEOBJECT_UPDATE
	std::cout << "GO Update Average:\t\t" << GetAverage(goUpdateTimes) << " milliseconds\n\n";
#endif // GAMEOBJECT_UPDATE

#endif // GAMEOBJECT

#ifdef ENTT

#ifdef ENTT_CREATION
	std::cout << "ENTT Init Average:\t\t" << GetAverage(enttInitTimes) << " milliseconds\n";
#endif // ENTT_CREATION

#ifdef ENTT_UPDATE
	std::cout << "ENTT Update Average:\t\t" << GetAverage(enttUpdateTimes) << " milliseconds\n\n";
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

	//SECTION("Making 10 entities and testing their updates")
	//{
	//	constexpr int size{ 10 };
	//	float startPositions[size]{};

	//	for (int i{}; i < size; ++i)
	//	{
	//		ECS::Entity entity{ registry.CreateEntity() };

	//		registry.AddComponent<GravityComponent>(entity);
	//		registry.AddComponent<TransformComponent>(entity);
	//		registry.AddComponent<RigidBodyComponent>(entity);

	//		startPositions[i] = registry.GetComponent<TransformComponent>(entity).Position.y;
	//	}

	//	auto view = registry.CreateView<GravityComponent, TransformComponent>();

	//	view.ForEach([](const auto& grav, auto& trans)
	//		{
	//			trans.Position.y += grav.Gravity; // - 981.0
	//		});

	//	for (int i{}; i < size; ++i)
	//	{
	//		REQUIRE(startPositions[i] > registry.GetComponent<TransformComponent>(i).Position.y);
	//	}
	//}

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