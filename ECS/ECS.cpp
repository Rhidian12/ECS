// ECS.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include "ECSConstants.h"

#include <iostream>

#include "System/System.h"
#include "Component/Component.h"

#include "GOComponent/GOComponent.h"
#include "GameObject/GameObject.h"

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

void GravityUpdate(const ECS::System& system)
{
	auto view = system.CreateView<GravityComponent, RigidBodyComponent>();

	view.ForEach([](auto& gravity, auto& rigidBody)->void
		{
			rigidBody.Velocity.y += gravity.Gravity * rigidBody.Mass;
		});
}
void PhysicsUpdate(const ECS::System& system)
{
	auto view = system.CreateView<RigidBodyComponent, TransformComponent>();

	view.ForEach([](auto& rigidBody, auto& transform)->void
		{
			transform.Position.x += rigidBody.Velocity.x;
			transform.Position.y += rigidBody.Velocity.y;
		});
}

/* Defines! */
//#define GAMEOBJECT
// #define ENTT
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

/* Things to test */
#ifdef CUSTOMECS
ECS::System g_GravitySystem{};
ECS::System g_PhysicsSystem{};
#endif
#ifdef GAMEOBJECT
std::vector<GO::GameObject*> g_GameObjects;
#endif
#ifdef ENTT
entt::registry g_GravityRegistry;
entt::registry g_PhysicsRegistry;
std::vector<entt::entity> g_enttEntities{};
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
void TestInitECS(const ECS::Entity amount)
{
	using namespace ECS;

	std::chrono::steady_clock::time_point t1{};
	std::chrono::steady_clock::time_point t2{};

	if (g_GravitySystem.GetAmountOfEntities() > 0)
	{
		g_GravitySystem.ReleaseEntities();
		g_PhysicsSystem.ReleaseEntities();
	}

	t1 = std::chrono::steady_clock::now();

	for (int i{}; i < amount; ++i)
	{
		Entity entity{ g_GravitySystem.CreateEntity() };

		g_GravitySystem.AddComponent<TransformComponent>(entity);
		g_GravitySystem.AddComponent<RigidBodyComponent>(entity);
		g_GravitySystem.AddComponent<GravityComponent>(entity);

		g_PhysicsSystem.AddEntity(entity);
		g_PhysicsSystem.AddComponent<RigidBodyComponent>(entity);
		g_PhysicsSystem.AddComponent<TransformComponent>(entity);
	}

	t2 = std::chrono::steady_clock::now();

	g_ECSInitTimes.push_back(std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count());
}
void TestUpdateECS()
{
	using namespace ECS;

	std::chrono::steady_clock::time_point t1{};
	std::chrono::steady_clock::time_point t2{};

	t1 = std::chrono::steady_clock::now();

	GravityUpdate(g_GravitySystem);
	PhysicsUpdate(g_PhysicsSystem);

	t2 = std::chrono::steady_clock::now();

	g_ECSUpdateTimes.push_back(std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count());
}
#endif
#ifdef GAMEOBJECT
void TestInitGO(const ECS::Entity amount)
{
	using namespace GO;

	std::chrono::steady_clock::time_point t1{};
	std::chrono::steady_clock::time_point t2{};

	t1 = std::chrono::steady_clock::now();

	for (int i{}; i < amount; ++i)
	{
		GameObject* pG{ new GameObject{} };

		pG->AddComponent(new GOGravityComponent{});
		pG->AddComponent(new GORigidBodyComponent{ pG->GetComponent<GOGravityComponent>() });
		pG->AddComponent(new GOTransformComponent{ pG->GetComponent<GORigidBodyComponent>() });

		g_GameObjects.push_back(pG);
	}

	t2 = std::chrono::steady_clock::now();

	g_GOInitTimes.push_back(std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count());
}
void TestUpdateGO()
{
	using namespace GO;

	std::chrono::steady_clock::time_point t1{};
	std::chrono::steady_clock::time_point t2{};

	t1 = std::chrono::steady_clock::now();

	for (GameObject* const pG : g_GameObjects)
	{
		pG->Update();
	}

	t2 = std::chrono::steady_clock::now();

	g_GOUpdateTimes.push_back(std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count());
}
#endif
#ifdef ENTT
void TestInitENTT(const ECS::Entity amount)
{
	using namespace entt;

	std::chrono::steady_clock::time_point t1{};
	std::chrono::steady_clock::time_point t2{};

	g_GravityRegistry.destroy(g_GravityRegistry.data(), g_GravityRegistry.data() + g_GravityRegistry.size());
	g_PhysicsRegistry.destroy(g_PhysicsRegistry.data(), g_PhysicsRegistry.data() + g_PhysicsRegistry.size());
	g_enttEntities.clear();

	t1 = std::chrono::steady_clock::now();

	for (int i{}; i < amount; ++i)
	{
		auto enttEntity{ g_GravityRegistry.create() };
		g_GravityRegistry.emplace<ENTTGravity>(enttEntity);
		g_GravityRegistry.emplace<ENTTTransformComponent>(enttEntity);
		g_GravityRegistry.emplace<ENTTRigidBodyComponent>(enttEntity);

		enttEntity = g_PhysicsRegistry.create();
		g_PhysicsRegistry.emplace<ENTTTransformComponent>(enttEntity);
		g_PhysicsRegistry.emplace<ENTTRigidBodyComponent>(enttEntity);

		g_enttEntities.push_back(enttEntity);
	}

	t2 = std::chrono::steady_clock::now();

	g_enttInitTimes.push_back(std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count());
}
void TestUpdateENTT()
{
	std::chrono::steady_clock::time_point t1{};
	std::chrono::steady_clock::time_point t2{};

	t1 = std::chrono::steady_clock::now();

	ENTTGravityUpdate(g_GravityRegistry);
	ENTTPhysicsUpdate(g_PhysicsRegistry);

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
	constexpr Entity AmountOfEntities{ 10'00 };
	constexpr int Iterations{ 50 };

	/* Initialize different systems */
	for (int i{}; i < Iterations; ++i)
	{
#ifdef CUSTOMECS
		TestInitECS(AmountOfEntities);
#endif
#ifdef GAMEOBJECT
		TestInitGO(AmountOfEntities);
#endif
#ifdef ENTT
		TestInitENTT(AmountOfEntities);
#endif
	}

	/* Test different systems */
	for (int i{}; i < Iterations; ++i)
	{
#ifdef CUSTOMECS
		TestUpdateECS();
#endif
#ifdef GAMEOBJECT
		TestUpdateGO();
#endif
#ifdef ENTT
		TestUpdateENTT();
#endif
	}

#if defined(CUSTOMECS) && defined(ENTT)
	for (int i{}; i < AmountOfEntities; ++i)
	{
		auto ecs = g_GravitySystem.GetComponent<RigidBodyComponent>(i);
		auto entt = g_GravityRegistry.get<ENTTRigidBodyComponent>(g_enttEntities[i]);

		if (!Utils::Equals(ecs.Velocity.y, entt.Velocity.y))
		{
			std::abort();
		}
	}
#endif

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
	for (GameObject* pG : g_GameObjects)
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
		REQUIRE(set.Find(5) == 0); /* should be the first element */
	}

	SECTION("Add and Remove 1 value")
	{
		set.Add(5);

		REQUIRE(set.Contains(5));
		REQUIRE(set.Size() == 1);
		REQUIRE(set.Find(5) == 0); /* should be the first element */

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

	SECTION("Adding 10 000 elements")
	{
		for (int i{}; i < 10'000; ++i)
		{
			set.Add(i);
		}

		REQUIRE(set.Size() == 10'000);
		REQUIRE(set.Find(0) == 0);
		REQUIRE(set.Contains(7500));
		REQUIRE(set.Contains(1));
		REQUIRE(set.Contains(9999));
		REQUIRE(set.Contains(5000));

		set.Clear();

		REQUIRE(set.Size() == 0);
	}

	SECTION("Adding max elements")
	{
		for (int i{}; i < std::numeric_limits<uint16_t>::max(); ++i)
		{
			set.Add(i);
		}

		REQUIRE(set.Size() == std::numeric_limits<uint16_t>::max());
		REQUIRE(set.Find(0) == 0);
		REQUIRE(set.Contains(7500));
		REQUIRE(set.Contains(1));
		REQUIRE(set.Contains(9999));
		REQUIRE(set.Contains(5000));

		set.Clear();

		REQUIRE(set.Size() == 0);
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
	ECS::System gravitySystem{};

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

		REQUIRE(ECS::ComponentManager::GetInstance()->GetNrOfComponents<GravityComponent>() == 1);
		REQUIRE(ECS::ComponentManager::GetInstance()->GetNrOfComponents<RigidBodyComponent>() == 1);
		REQUIRE(ECS::ComponentManager::GetInstance()->GetNrOfComponents<TransformComponent>() == 1);
	}

	SECTION("Adding and Release Entities")
	{
		ECS::Entity entity{ gravitySystem.CreateEntity() };

		REQUIRE(gravitySystem.GetAmountOfEntities() == 1);

		gravitySystem.RemoveEntitiy(entity);

		REQUIRE(gravitySystem.GetAmountOfEntities() == 0);
	}

	SECTION("Adding and Removing components")
	{
		ECS::Entity entity{ gravitySystem.CreateEntity() };

		gravitySystem.AddComponent<GravityComponent>(entity);
		gravitySystem.AddComponent<RigidBodyComponent>(entity);
		gravitySystem.AddComponent<TransformComponent>(entity);

		REQUIRE(ECS::ComponentManager::GetInstance()->GetNrOfComponents<GravityComponent>() == 1);
		REQUIRE(ECS::ComponentManager::GetInstance()->GetNrOfComponents<RigidBodyComponent>() == 1);
		REQUIRE(ECS::ComponentManager::GetInstance()->GetNrOfComponents<TransformComponent>() == 1);

		gravitySystem.RemoveComponent<GravityComponent>(entity);
		REQUIRE(ECS::ComponentManager::GetInstance()->GetNrOfComponents<GravityComponent>() == 0);

		gravitySystem.RemoveComponent<RigidBodyComponent>(entity);
		REQUIRE(ECS::ComponentManager::GetInstance()->GetNrOfComponents<RigidBodyComponent>() == 0);

		gravitySystem.RemoveComponent<TransformComponent>(entity);
		REQUIRE(ECS::ComponentManager::GetInstance()->GetNrOfComponents<TransformComponent>() == 0);
	}

	SECTION("Testing Component Update")
	{
		ECS::Entity entity{ gravitySystem.CreateEntity() };

		gravitySystem.AddComponent<GravityComponent>(entity);
		gravitySystem.AddComponent<RigidBodyComponent>(entity);

		GravityUpdate(gravitySystem);

		REQUIRE(!ECS::Utils::Equals(gravitySystem.GetComponent<RigidBodyComponent>(entity).Velocity.y, 0.f));
	}

	SECTION("Testing 1000 Component Update")
	{
		for (int i{}; i < 1000; ++i)
		{
			ECS::Entity entity{ gravitySystem.CreateEntity() };

			gravitySystem.AddComponent<GravityComponent>(entity);
			gravitySystem.AddComponent<RigidBodyComponent>(entity);
			gravitySystem.AddComponent<TransformComponent>(entity);
		}

		REQUIRE(ECS::ComponentManager::GetInstance()->GetNrOfComponents<GravityComponent>() == 1000);
		REQUIRE(ECS::ComponentManager::GetInstance()->GetNrOfComponents<RigidBodyComponent>() == 1000);
		REQUIRE(ECS::ComponentManager::GetInstance()->GetNrOfComponents<TransformComponent>() == 1000);

		GravityUpdate(gravitySystem);

		for (const ECS::Entity entity : gravitySystem.GetEntities())
		{
			REQUIRE(!ECS::Utils::Equals(gravitySystem.GetComponent<RigidBodyComponent>(entity).Velocity.y, 0.f));
		}
	}

	SECTION("The previous Section should have cleared all Components")
	{
		REQUIRE(ECS::ComponentManager::GetInstance()->GetNrOfComponents<GravityComponent>() == 0);
		REQUIRE(ECS::ComponentManager::GetInstance()->GetNrOfComponents<RigidBodyComponent>() == 0);
		REQUIRE(ECS::ComponentManager::GetInstance()->GetNrOfComponents<TransformComponent>() == 0);
	}

	/* This sections throws an assert */
	SECTION("Testing what happens with a system that contains too little entities' components")
	{
		ECS::Entity entity{ gravitySystem.CreateEntity() };

		gravitySystem.AddComponent<GravityComponent>(entity);

		/* Uncommenting this line should result in an assert failure */
		// GravityUpdate(gravitySystem);
	}

	SECTION("Making ten entities and adding components to all of those")
	{
		for (int i{}; i < 10; ++i)
		{
			ECS::Entity entity{ gravitySystem.CreateEntity() };

			gravitySystem.AddComponent<GravityComponent>(entity);
			gravitySystem.AddComponent<RigidBodyComponent>(entity);
			gravitySystem.AddComponent<TransformComponent>(entity);
		}

		REQUIRE(ECS::ComponentManager::GetInstance()->GetNrOfComponents<GravityComponent>() == 10);
		REQUIRE(ECS::ComponentManager::GetInstance()->GetNrOfComponents<RigidBodyComponent>() == 10);
		REQUIRE(ECS::ComponentManager::GetInstance()->GetNrOfComponents<TransformComponent>() == 10);
	}

	SECTION("Making 100 entities and adding components to all of those")
	{
		for (int i{}; i < 100; ++i)
		{
			ECS::Entity entity{ gravitySystem.CreateEntity() };

			gravitySystem.AddComponent<GravityComponent>(entity);
			gravitySystem.AddComponent<RigidBodyComponent>(entity);
			gravitySystem.AddComponent<TransformComponent>(entity);
		}

		REQUIRE(ECS::ComponentManager::GetInstance()->GetNrOfComponents<GravityComponent>() == 100);
		REQUIRE(ECS::ComponentManager::GetInstance()->GetNrOfComponents<RigidBodyComponent>() == 100);
		REQUIRE(ECS::ComponentManager::GetInstance()->GetNrOfComponents<TransformComponent>() == 100);
	}

	SECTION("Making 1000 entities and adding components to all of those")
	{
		for (int i{}; i < 1000; ++i)
		{
			ECS::Entity entity{ gravitySystem.CreateEntity() };

			gravitySystem.AddComponent<GravityComponent>(entity);
			gravitySystem.AddComponent<RigidBodyComponent>(entity);
			gravitySystem.AddComponent<TransformComponent>(entity);
		}

		REQUIRE(ECS::ComponentManager::GetInstance()->GetNrOfComponents<GravityComponent>() == 1000);
		REQUIRE(ECS::ComponentManager::GetInstance()->GetNrOfComponents<RigidBodyComponent>() == 1000);
		REQUIRE(ECS::ComponentManager::GetInstance()->GetNrOfComponents<TransformComponent>() == 1000);
	}

	SECTION("Making 10 000 entities and adding components to all of those")
	{
		for (int i{}; i < 10'000; ++i)
		{
			ECS::Entity entity{ gravitySystem.CreateEntity() };

			gravitySystem.AddComponent<GravityComponent>(entity);
			gravitySystem.AddComponent<RigidBodyComponent>(entity);
			gravitySystem.AddComponent<TransformComponent>(entity);
		}

		REQUIRE(ECS::ComponentManager::GetInstance()->GetNrOfComponents<GravityComponent>() == 10'000);
		REQUIRE(ECS::ComponentManager::GetInstance()->GetNrOfComponents<RigidBodyComponent>() == 10'000);
		REQUIRE(ECS::ComponentManager::GetInstance()->GetNrOfComponents<TransformComponent>() == 10'000);
	}

	//SECTION("Making max entities and adding components to all of those")
	//{
	//	for (int i{}; i < ECS::MaxEntities; ++i)
	//	{
	//		ECS::Entity entity{ gravitySystem.CreateEntity() };

	//		gravitySystem.AddComponent<GravityComponent>(entity);
	//		gravitySystem.AddComponent<RigidBodyComponent>(entity);
	//		gravitySystem.AddComponent<TransformComponent>(entity);
	//	}

	//	REQUIRE(ECS::ComponentManager::GetInstance()->GetComponents<GravityComponent>().size() == ECS::MaxEntities);
	//	REQUIRE(ECS::ComponentManager::GetInstance()->GetComponents<RigidBodyComponent>().size() == ECS::MaxEntities);
	//	REQUIRE(ECS::ComponentManager::GetInstance()->GetComponents<TransformComponent>().size() == ECS::MaxEntities);
	//}
}
#endif