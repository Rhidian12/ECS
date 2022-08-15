// ECS.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include "ECSConstants.h"

#include "Registry/Registry.h"

#include "GOComponent/GOComponent.h"
#include "GameObject/GameObject.h"

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

/* Defines! */
#define GAMEOBJECT
#define ENTT
#define CUSTOMECS

//#define WRITE_TO_FILE
#define APPEND_TO_FILE
// #define OVERWRITE_TO_FILE

//#define UNIT_TESTS
#define BENCHMARKS

#ifdef UNIT_TESTS
#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#endif

#ifdef CUSTOMECS
std::deque<long long> g_ECSUpdateTimes{};
std::deque<long long> g_ECSInitTimes{};
#endif
#ifdef GAMEOBJECT
std::deque<long long> g_GOUpdateTimes{};
std::deque<long long> g_GOInitTimes{};
#endif
#ifdef ENTT
std::deque<long long> g_enttUpdateTimes{};
std::deque<long long> g_enttInitTimes{};
#endif

#ifdef CUSTOMECS
ECS::Registry TestInitECS(const ECS::Entity amount)
{
	using namespace ECS;

	std::chrono::steady_clock::time_point t1{}, t2{};

	t1 = std::chrono::steady_clock::now();

	Registry registry{};

	for (size_t i{}; i < amount; ++i)
	{
		Entity entity{ registry.CreateEntity() };

		registry.AddComponent<TransformComponent>(entity);
		registry.AddComponent<RigidBodyComponent>(entity);
		registry.AddComponent<GravityComponent>(entity);
	}

	t2 = std::chrono::steady_clock::now();

	g_ECSInitTimes.push_back(std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count());

	return registry;
}
void TestUpdateECS(ECS::Registry& registry)
{
	using namespace ECS;

	std::chrono::steady_clock::time_point t1{};
	std::chrono::steady_clock::time_point t2{};

	t1 = std::chrono::steady_clock::now();

	GravityUpdate(registry);
	PhysicsUpdate(registry);

	t2 = std::chrono::steady_clock::now();

	g_ECSUpdateTimes.push_back(std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count());
}
#endif
#ifdef GAMEOBJECT
std::vector<GO::GameObject*> TestInitGO(const ECS::Entity amount)
{
	using namespace GO;

	std::chrono::steady_clock::time_point t1{};
	std::chrono::steady_clock::time_point t2{};

	t1 = std::chrono::steady_clock::now();

	std::vector<GO::GameObject*> gameObjects{};

	for (size_t i{}; i < amount; ++i)
	{
		GameObject* pG{ new GameObject{} };

		pG->AddComponent(new GOGravityComponent{});
		pG->AddComponent(new GORigidBodyComponent{ pG->GetComponent<GOGravityComponent>() });
		pG->AddComponent(new GOTransformComponent{ pG->GetComponent<GORigidBodyComponent>() });

		gameObjects.push_back(pG);
	}

	t2 = std::chrono::steady_clock::now();

	g_GOInitTimes.push_back(std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count());

	return gameObjects;
}
void TestUpdateGO(std::vector<GO::GameObject*>& gameObjects)
{
	using namespace GO;

	std::chrono::steady_clock::time_point t1{};
	std::chrono::steady_clock::time_point t2{};

	t1 = std::chrono::steady_clock::now();

	for (GameObject* const pG : gameObjects)
	{
		pG->Update();
	}

	t2 = std::chrono::steady_clock::now();

	g_GOUpdateTimes.push_back(std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count());
}
#endif
#ifdef ENTT
entt::registry TestInitENTT(const ECS::Entity amount)
{
	using namespace entt;

	std::chrono::steady_clock::time_point t1{}, t2{};

	t1 = std::chrono::steady_clock::now();

	entt::registry registry{};

	for (size_t i{}; i < amount; ++i)
	{
		auto enttEntity{ registry.create() };
		registry.emplace<ENTTGravity>(enttEntity);
		registry.emplace<ENTTTransformComponent>(enttEntity);
		registry.emplace<ENTTRigidBodyComponent>(enttEntity);
	}

	t2 = std::chrono::steady_clock::now();

	g_enttInitTimes.push_back(std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count());

	return registry;
}
void TestUpdateENTT(entt::registry& registry)
{
	std::chrono::steady_clock::time_point t1{};
	std::chrono::steady_clock::time_point t2{};

	t1 = std::chrono::steady_clock::now();

	ENTTGravityUpdate(registry);
	ENTTPhysicsUpdate(registry);

	t2 = std::chrono::steady_clock::now();

	g_enttUpdateTimes.push_back(std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count());
}
#endif

#ifdef BENCHMARKS
int main(int*, char* [])
{
	using namespace ECS;
	using namespace GO;

	/* Benchmarking Constants */
	constexpr Entity AmountOfEntities{ 100'000 };
	constexpr int Iterations{ 1 };
	constexpr float TimeToUpdate{ 5.f };

#ifdef CUSTOMECS
	ECS::Registry ECS{};
#endif
#ifdef GAMEOBJECT
	std::vector<GO::GameObject*> gameObjects{};
#endif
#ifdef ENTT
	entt::registry entt{};
#endif

	/* Initialize different systems */
	for (int i{}; i < Iterations; ++i)
	{
#ifdef CUSTOMECS
		ECS::Registry registry{ TestInitECS(AmountOfEntities) };
		if (i == Iterations - 1)
		{
			ECS = std::move(registry);
		}
#endif
#ifdef GAMEOBJECT
		std::vector<GO::GameObject*> gos = TestInitGO(AmountOfEntities);
		if (i == Iterations - 1)
		{
			gameObjects = std::move(gos);
		}
		else
		{
			for (auto pG : gos)
			{
				delete pG;
			}
		}
#endif
#ifdef ENTT
		entt::registry enttRegistry = TestInitENTT(AmountOfEntities);
		if (i == Iterations - 1)
		{
			entt = std::move(enttRegistry);
		}
#endif
	}

	/* Test different systems */
	float deltaTime{};
	std::chrono::steady_clock::time_point now{ std::chrono::steady_clock::now() };

	while (deltaTime < TimeToUpdate)
	{
#ifdef CUSTOMECS
		TestUpdateECS(ECS);
#endif
#ifdef GAMEOBJECT
		TestUpdateGO(gameObjects);
#endif
#ifdef ENTT
		TestUpdateENTT(entt);
#endif

		deltaTime += std::chrono::duration<float>(std::chrono::steady_clock::now() - now).count();

		now = std::chrono::steady_clock::now();
	}

	/* Cleanup results */
#ifdef CUSTOMECS
	std::sort(g_ECSUpdateTimes.begin(), g_ECSUpdateTimes.end());
	std::sort(g_ECSInitTimes.begin(), g_ECSInitTimes.end());
#endif
#ifdef GAMEOBJECT
	std::sort(g_GOUpdateTimes.begin(), g_GOUpdateTimes.end());
	std::sort(g_GOInitTimes.begin(), g_GOInitTimes.end());
#endif
#ifdef ENTT
	std::sort(g_enttUpdateTimes.begin(), g_enttUpdateTimes.end());
	std::sort(g_enttInitTimes.begin(), g_enttInitTimes.end());
#endif
	if (Iterations / 10 > 0)
	{
		for (int i{}; i < Iterations / 10; ++i)
		{
#ifdef CUSTOMECS
			g_ECSUpdateTimes.pop_back();
			g_ECSUpdateTimes.pop_front();

			g_ECSInitTimes.pop_back();
			g_ECSInitTimes.pop_front();
#endif
#ifdef GAMEOBJECT
			g_GOUpdateTimes.pop_back();
			g_GOUpdateTimes.pop_front();

			g_GOInitTimes.pop_back();
			g_GOInitTimes.pop_front();
#endif
#ifdef ENTT
			g_enttUpdateTimes.pop_back();
			g_enttUpdateTimes.pop_front();

			g_enttInitTimes.pop_back();
			g_enttInitTimes.pop_front();
#endif
		}
	}

	/* Print results */
	std::cout << "Amount of entities: " << AmountOfEntities << "\n";
	std::cout << "Iterations: " << Iterations << "\n\n";
	std::cout << "Update Run Time: " << TimeToUpdate << " seconds\n\n";

#ifdef CUSTOMECS
	std::cout << "ECS Init Average:\t\t" << std::accumulate(g_ECSInitTimes.cbegin(), g_ECSInitTimes.cend(), (long long)0) / g_ECSInitTimes.size() << " nanoseconds\n";
#endif
#ifdef GAMEOBJECT
	std::cout << "GO Init Average:\t\t" << std::accumulate(g_GOInitTimes.cbegin(), g_GOInitTimes.cend(), (long long)0) / g_GOInitTimes.size() << " nanoseconds\n";
#endif
#ifdef ENTT
	std::cout << "ENTT Init Average:\t\t" << std::accumulate(g_enttInitTimes.cbegin(), g_enttInitTimes.cend(), (long long)0) / g_enttInitTimes.size() << " nanoseconds\n\n";
#endif
#ifdef CUSTOMECS
	std::cout << "ECS Update Average:\t\t" << std::accumulate(g_ECSUpdateTimes.cbegin(), g_ECSUpdateTimes.cend(), (long long)0) / g_ECSUpdateTimes.size() << " nanoseconds\n";
#endif
#ifdef GAMEOBJECT
	std::cout << "GO Update Average:\t\t" << std::accumulate(g_GOUpdateTimes.cbegin(), g_GOUpdateTimes.cend(), (long long)0) / g_GOUpdateTimes.size() << " nanoseconds\n";
#endif
#ifdef ENTT
	std::cout << "ENTT Update Average:\t\t" << std::accumulate(g_enttUpdateTimes.cbegin(), g_enttUpdateTimes.cend(), (long long)0) / g_enttUpdateTimes.size() << " nanoseconds\n";
#endif

#ifdef CUSTOMECS
	std::cout << "\nECS Total:\t\t\t" << std::accumulate(g_ECSInitTimes.cbegin(), g_ECSInitTimes.cend(), (long long)0) + std::accumulate(g_ECSUpdateTimes.cbegin(), g_ECSUpdateTimes.cend(), (long long)0) << " nanoseconds\n";
#endif
#ifdef GAMEOBJECT
	std::cout << "GO Total:\t\t\t" << std::accumulate(g_GOInitTimes.cbegin(), g_GOInitTimes.cend(), (long long)0) + std::accumulate(g_GOUpdateTimes.cbegin(), g_GOUpdateTimes.cend(), (long long)0) << " nanoseconds\n";
#endif
#ifdef ENTT
	std::cout << "ENTT Total:\t\t\t" << std::accumulate(g_enttInitTimes.cbegin(), g_enttInitTimes.cend(), (long long)0) + std::accumulate(g_enttUpdateTimes.cbegin(), g_enttUpdateTimes.cend(), (long long)0) << " nanoseconds\n\n";
#endif

#ifdef WRITE_TO_FILE
	/* Write results to file: */
	std::fstream file{};

#ifdef APPEND_TO_FILE
	file.open("Benchmarks.txt", std::ios::out | std::ios::app);
#elif defined(OVERWRITE_TO_FILE)
	file.open("Benchmarks.txt", std::ios::out);
#endif

	if (file.is_open())
	{
		file << "Program Created " << AmountOfEntities << " Entities\n";
		file << "Program ran update code " << Iterations << " amount of times\n";

#ifdef CUSTOMECS
		file << "ECS Init Average: " << std::accumulate(g_ECSInitTimes.cbegin(), g_ECSInitTimes.cend(), (long long)0) / g_ECSInitTimes.size() << " nanoseconds\n";
#endif
#ifdef GAMEOBJECT
		file << "GO Init Average: " << std::accumulate(g_GOInitTimes.cbegin(), g_GOInitTimes.cend(), (long long)0) / g_GOInitTimes.size() << " nanoseconds\n";
#endif
#ifdef ENTT
		file << "ENTT Init Average: " << std::accumulate(g_enttInitTimes.cbegin(), g_enttInitTimes.cend(), (long long)0) / g_enttInitTimes.size() << " nanoseconds\n\n";
#endif
#ifdef CUSTOMECS
		file << "ECS Update Average: " << std::accumulate(g_ECSUpdateTimes.cbegin(), g_ECSUpdateTimes.cend(), (long long)0) / g_ECSUpdateTimes.size() << " nanoseconds\n\n";
#endif
#ifdef GAMEOBJECT
		file << "GO Update Average: " << std::accumulate(g_GOUpdateTimes.cbegin(), g_GOUpdateTimes.cend(), (long long)0) / g_GOUpdateTimes.size() << " nanoseconds\n\n";
#endif
#ifdef ENTT
		file << "ENTT Update Average: " << std::accumulate(g_enttUpdateTimes.cbegin(), g_enttUpdateTimes.cend(), (long long)0) / g_enttUpdateTimes.size() << " nanoseconds\n\n";
#endif
}
#endif

	/* Cleanup systems */
#ifdef GAMEOBJECT
	for (GameObject* pG : gameObjects)
		delete pG;
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