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

	return (t2 - t1).Count();
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
#define GAMEOBJECT
#define ENTT
#define CUSTOMECS

//#define UNIT_TESTS
#define BENCHMARKS

#ifdef UNIT_TESTS
#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#endif

#ifdef BENCHMARKS
int main(int*, char* [])
{
	using namespace ECS;
	using namespace GO;

	/* Benchmarking Constants */
	constexpr int Iterations{ 1 };
	constexpr Entity AmountOfEntities{ 100'000 };
	constexpr float TimeToUpdate{ 5.f };

#ifdef CUSTOMECS
	std::deque<double> ecsUpdateTimes{};
	std::deque<double> ecsInitTimes{};
#endif
#ifdef GAMEOBJECT
	std::deque<double> goUpdateTimes{};
	std::deque<double> goInitTimes{};
#endif
#ifdef ENTT
	std::deque<double> enttUpdateTimes{};
	std::deque<double> enttInitTimes{};
#endif

	for (int i{}; i < Iterations; ++i)
	{
#ifdef CUSTOMECS
		ECS::Registry ECS{};
		ecsInitTimes.push_back(Benchmark([AmountOfEntities, &ECS]()
			{
				for (size_t i{}; i < AmountOfEntities; ++i)
				{
					Entity entity{ ECS.CreateEntity() };

					ECS.AddComponent<TransformComponent>(entity);
					ECS.AddComponent<RigidBodyComponent>(entity);
					ECS.AddComponent<GravityComponent>(entity);
				}
			}));
#endif
#ifdef GAMEOBJECT
		std::vector<GO::GameObject*> gameObjects{};
		goInitTimes.push_back(Benchmark([AmountOfEntities, &gameObjects]()
			{
				for (size_t i{}; i < AmountOfEntities; ++i)
				{
					GameObject* pG{ new GameObject{} };

					pG->AddComponent(new GOGravityComponent{});
					pG->AddComponent(new GORigidBodyComponent{ pG->GetComponent<GOGravityComponent>() });
					pG->AddComponent(new GOTransformComponent{ pG->GetComponent<GORigidBodyComponent>() });

					gameObjects.push_back(pG);
				}
			}));
#endif
#ifdef ENTT
		entt::registry entt{};
		enttInitTimes.push_back(Benchmark([AmountOfEntities, &entt]()
			{
				for (size_t i{}; i < AmountOfEntities; ++i)
				{
					auto enttEntity{ entt.create() };
					entt.emplace<ENTTGravity>(enttEntity);
					entt.emplace<ENTTTransformComponent>(enttEntity);
					entt.emplace<ENTTRigidBodyComponent>(enttEntity);
				}
			}));
#endif

		/* Test different systems */
		float deltaTime{};
		size_t nrOfLoops{};
		std::chrono::steady_clock::time_point now{ std::chrono::steady_clock::now() };

		while (deltaTime < TimeToUpdate)
		{
#ifdef CUSTOMECS
			ecsUpdateTimes.push_back(Benchmark([&ECS]()
				{
					GravityUpdate(ECS);
					PhysicsUpdate(ECS);
				}));
#endif
#ifdef GAMEOBJECT
			goUpdateTimes.push_back(Benchmark([&gameObjects]()
				{
					for (GameObject* const pG : gameObjects)
					{
						pG->Update();
					}
				}));
#endif
#ifdef ENTT
			enttUpdateTimes.push_back(Benchmark([&entt]()
				{
					ENTTGravityUpdate(entt);
					ENTTPhysicsUpdate(entt);
				}));
#endif
			deltaTime += std::chrono::duration<float>(std::chrono::steady_clock::now() - now).count();

			++nrOfLoops;
			now = std::chrono::steady_clock::now();
		}

		/* Cleanup results */
#ifdef CUSTOMECS
		std::sort(ecsUpdateTimes.begin(), ecsUpdateTimes.end());
		std::sort(ecsInitTimes.begin(), ecsInitTimes.end());
#endif
#ifdef GAMEOBJECT
		std::sort(goUpdateTimes.begin(), goUpdateTimes.end());
		std::sort(goInitTimes.begin(), goInitTimes.end());
#endif
#ifdef ENTT
		std::sort(enttUpdateTimes.begin(), enttUpdateTimes.end());
		std::sort(enttInitTimes.begin(), enttInitTimes.end());
#endif
		for (size_t i{}; i < nrOfLoops / 10u; ++i)
		{
#ifdef CUSTOMECS
			ecsUpdateTimes.pop_back();
			ecsUpdateTimes.pop_front();

			ecsInitTimes.pop_back();
			ecsInitTimes.pop_front();
#endif
#ifdef GAMEOBJECT
			goUpdateTimes.pop_back();
			goUpdateTimes.pop_front();

			goInitTimes.pop_back();
			goInitTimes.pop_front();
#endif
#ifdef ENTT
			enttUpdateTimes.pop_back();
			enttUpdateTimes.pop_front();

			enttInitTimes.pop_back();
			enttInitTimes.pop_front();
#endif
		}

		/* Cleanup systems */
#ifdef GAMEOBJECT
		for (GameObject* pG : gameObjects)
			delete pG;
#endif
	}

	/* Print results */
	std::cout << "Amount of entities: " << AmountOfEntities << "\n";
	std::cout << "Iterations: " << Iterations << "\n\n";
	std::cout << "Update Run Time: " << TimeToUpdate << " seconds\n\n";

#ifdef CUSTOMECS
	std::cout << "ECS Init Average:\t\t" << std::accumulate(ecsInitTimes.cbegin(), ecsInitTimes.cend(), 0.0) / ecsInitTimes.size() << " milliseconds\n";
#endif
#ifdef GAMEOBJECT
	std::cout << "GO Init Average:\t\t" << std::accumulate(goInitTimes.cbegin(), goInitTimes.cend(), 0.0) / goInitTimes.size() << " milliseconds\n";
#endif
#ifdef ENTT
	std::cout << "ENTT Init Average:\t\t" << std::accumulate(enttInitTimes.cbegin(), enttInitTimes.cend(), 0.0) / enttInitTimes.size() << " milliseconds\n\n";
#endif
#ifdef CUSTOMECS
	std::cout << "ECS Update Average:\t\t" << std::accumulate(ecsUpdateTimes.cbegin(), ecsUpdateTimes.cend(), 0.0) / ecsUpdateTimes.size() << " milliseconds\n";
#endif
#ifdef GAMEOBJECT
	std::cout << "GO Update Average:\t\t" << std::accumulate(goUpdateTimes.cbegin(), goUpdateTimes.cend(), 0.0) / goUpdateTimes.size() << " milliseconds\n";
#endif
#ifdef ENTT
	std::cout << "ENTT Update Average:\t\t" << std::accumulate(enttUpdateTimes.cbegin(), enttUpdateTimes.cend(), 0.0) / enttUpdateTimes.size() << " milliseconds\n";
#endif

#ifdef CUSTOMECS
	std::cout << "\nECS Total:\t\t\t" << std::accumulate(ecsInitTimes.cbegin(), ecsInitTimes.cend(), 0.0) + std::accumulate(ecsUpdateTimes.cbegin(), ecsUpdateTimes.cend(), 0.0) << " milliseconds\n";
#endif
#ifdef GAMEOBJECT
	std::cout << "GO Total:\t\t\t" << std::accumulate(goInitTimes.cbegin(), goInitTimes.cend(), 0.0) + std::accumulate(goUpdateTimes.cbegin(), goUpdateTimes.cend(), 0.0) << " milliseconds\n";
#endif
#ifdef ENTT
	std::cout << "ENTT Total:\t\t\t" << std::accumulate(enttInitTimes.cbegin(), enttInitTimes.cend(), 0.0) + std::accumulate(enttUpdateTimes.cbegin(), enttUpdateTimes.cend(), 0.0) << " milliseconds\n\n";
#endif

	return 0;
}
#endif

#ifdef UNIT_TESTS
TEST_CASE("Testing SparseSet")
{
	ECS::SparseSet<uint16_t> set{};

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

	SECTION("Adding Max elements and removing certain elements")
	{
		for (int i{}; i < std::numeric_limits<uint16_t>::max(); ++i)
		{
			set.Add(i);
		}

		REQUIRE(set.Size() == std::numeric_limits<uint16_t>::max());

		for (int i{}; i < 10'000; ++i)
		{
			REQUIRE(set.Contains(i));
			set.Remove(i);
			REQUIRE(!set.Contains(i));
		}

		for (int i{}; i < 10'000; ++i)
		{
			REQUIRE(!set.Contains(i));
			set.Add(i);
			REQUIRE(set.Contains(i));
		}
	}
}

TEST_CASE("Testing custom ECS")
{
	ECS::EntityManager gravitySystem{};

	REQUIRE(gravitySystem.GetAmountOfEntities() == 0);

	SECTION("Making one entity")
	{
		ECS::Entity entity{ gravitySystem.CreateEntity() };

		/* for this one it's possible, but when entities get deleted, their id's get pushed back */
		REQUIRE(entity == 0);
	}

	SECTION("Making one entity and adding components to it")
	{
		ECS::Entity entity{ gravitySystem.CreateEntity() };

		gravitySystem.AddComponent<GravityComponent>(entity);
		gravitySystem.AddComponent<RigidBodyComponent>(entity);
		gravitySystem.AddComponent<TransformComponent>(entity);

		//REQUIRE(ECS::ComponentManager::GetInstance().GetNrOfComponents<GravityComponent>() == 1);
		//REQUIRE(ECS::ComponentManager::GetInstance().GetNrOfComponents<RigidBodyComponent>() == 1);
		//REQUIRE(ECS::ComponentManager::GetInstance().GetNrOfComponents<TransformComponent>() == 1);
	}

	SECTION("Adding and Release Entities")
	{
		ECS::Entity entity{ gravitySystem.CreateEntity() };

		REQUIRE(gravitySystem.GetAmountOfEntities() == 1);

		gravitySystem.ReleaseEntity(entity);

		REQUIRE(gravitySystem.GetAmountOfEntities() == 0);
	}

	SECTION("Adding and Removing components")
	{
		ECS::Entity entity{ gravitySystem.CreateEntity() };

		gravitySystem.AddComponent<GravityComponent>(entity);
		gravitySystem.AddComponent<RigidBodyComponent>(entity);
		gravitySystem.AddComponent<TransformComponent>(entity);

		// REQUIRE(ECS::ComponentManager::GetInstance()->GetNrOfComponents<GravityComponent>() == 1);
		// REQUIRE(ECS::ComponentManager::GetInstance()->GetNrOfComponents<RigidBodyComponent>() == 1);
		// REQUIRE(ECS::ComponentManager::GetInstance()->GetNrOfComponents<TransformComponent>() == 1);

		gravitySystem.RemoveComponent<GravityComponent>(entity);
		// REQUIRE(ECS::ComponentManager::GetInstance()->GetNrOfComponents<GravityComponent>() == 0);

		gravitySystem.RemoveComponent<RigidBodyComponent>(entity);
		// REQUIRE(ECS::ComponentManager::GetInstance()->GetNrOfComponents<RigidBodyComponent>() == 0);

		gravitySystem.RemoveComponent<TransformComponent>(entity);
		// REQUIRE(ECS::ComponentManager::GetInstance()->GetNrOfComponents<TransformComponent>() == 0);

		/* Removing a Component from a entity without that component throws assertion */
	}

	SECTION("Testing Component Update")
	{
		ECS::Entity entity{ gravitySystem.CreateEntity() };

		gravitySystem.AddComponent<GravityComponent>(entity);
		gravitySystem.AddComponent<RigidBodyComponent>(entity);

		GravityUpdate(gravitySystem);

		REQUIRE(!ECS::Utils::Equals(gravitySystem.GetComponent<RigidBodyComponent>(entity).Velocity.y, 0.f));
	}

	/* This sections throws an assert */
	SECTION("Testing what happens with a system that contains too little entities' components")
	{
		ECS::Entity entity{ gravitySystem.CreateEntity() };

		gravitySystem.AddComponent<GravityComponent>(entity);

		/* Uncommenting this line should result in an assert failure */
		// GravityUpdate(gravitySystem);
	}
}
#endif