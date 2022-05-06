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

void ENTTUpdate(entt::registry& registry)
{
	auto view = registry.view<const ENTTGravity, ENTTRigidBodyComponent, ENTTTransformComponent>();

	view.each([](const auto& gravity, auto& rigidBody, auto& transform)
		{
			rigidBody.Velocity.y += gravity.Gravity * rigidBody.Mass;

			transform.Position.x += rigidBody.Velocity.x;
			transform.Position.y += rigidBody.Velocity.y;
		});
}

void GravityUpdate(ECS::System& system)
{
	auto view = system.CreateView<GravityComponent, RigidBodyComponent, TransformComponent>();

	view.ForEach([](auto& gravity, auto& rigidBody, auto& transform)->void
		{
			rigidBody.Velocity.y += gravity.Gravity * rigidBody.Mass;

			transform.Position.x += rigidBody.Velocity.x;
			transform.Position.y += rigidBody.Velocity.y;
		});
}

/* Defines! */
#define GAMEOBJECT
#define CUSTOMECS
#define ENTT

#define WRITE_TO_FILE
// #define APPEND_TO_FILE
// #define OVERWRITE_TO_FILE

/* Things to test */
#ifdef CUSTOMECS
ECS::System g_GravitySystem;
#endif
#ifdef GAMEOBJECT
std::vector<GO::GameObject*> g_GameObjects;
#endif
#ifdef ENTT
entt::registry g_Registry;
#endif

#ifdef CUSTOMECS
std::deque<long long> g_ECSUpdateTimes{};
#endif
#ifdef GAMEOBJECT
std::deque<long long> g_GOUpdateTimes{};
#endif
#ifdef ENTT
std::deque<long long> g_enttUpdateTimes{};
#endif

#ifdef CUSTOMECS
void TestECS()
{
	using namespace ECS;

	std::chrono::steady_clock::time_point t1{};
	std::chrono::steady_clock::time_point t2{};

	t1 = std::chrono::steady_clock::now();

	GravityUpdate(g_GravitySystem);

	t2 = std::chrono::steady_clock::now();

	g_ECSUpdateTimes.push_back(std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count());
}
#endif
#ifdef GAMEOBJECT
void TestGO()
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
void TestENTT()
{
	std::chrono::steady_clock::time_point t1{};
	std::chrono::steady_clock::time_point t2{};

	t1 = std::chrono::steady_clock::now();

	ENTTUpdate(g_Registry);

	t2 = std::chrono::steady_clock::now();

	g_enttUpdateTimes.push_back(std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count());
}
#endif

int main(int*, char* [])
{
	using namespace ECS;
	using namespace GO;

	/* Benchmarking Constants */
	constexpr Entity AmountOfEntities{ MaxEntities };
	constexpr int Iterations{ 100 };

	/* Initialize different systems */
	for (int i{}; i < AmountOfEntities; ++i)
	{
#ifdef CUSTOMECS
		Entity entity{ g_GravitySystem.CreateEntity() };

		g_GravitySystem.AddComponent<TransformComponent>(entity);
		g_GravitySystem.AddComponent<RigidBodyComponent>(entity);
		g_GravitySystem.AddComponent<GravityComponent>(entity);
#endif
#ifdef GAMEOBJECT
		GameObject* pG{ new GameObject{} };

		pG->AddComponent(new GOGravityComponent{});
		pG->AddComponent(new GORigidBodyComponent{ pG->GetComponent<GOGravityComponent>() });
		pG->AddComponent(new GOTransformComponent{ pG->GetComponent<GORigidBodyComponent>() });

		g_GameObjects.push_back(pG);
#endif
#ifdef ENTT
		const auto enttEntity{ g_Registry.create() };
		g_Registry.emplace<ENTTGravity>(enttEntity);
		g_Registry.emplace<ENTTTransformComponent>(enttEntity);
		g_Registry.emplace<ENTTRigidBodyComponent>(enttEntity);
#endif
	}

	/* Test different systems */
	for (int i{}; i < Iterations; ++i)
	{
#ifdef CUSTOMECS
		TestECS();
#endif
#ifdef GAMEOBJECT
		TestGO();
#endif
#ifdef ENTT
		TestENTT();
#endif
	}

	/* Cleanup results */
#ifdef CUSTOMECS
	std::sort(g_ECSUpdateTimes.begin(), g_ECSUpdateTimes.end());
#endif
#ifdef GAMEOBJECT
	std::sort(g_GOUpdateTimes.begin(), g_GOUpdateTimes.end());
#endif
#ifdef ENTT
	std::sort(g_enttUpdateTimes.begin(), g_enttUpdateTimes.end());
#endif
	if (Iterations / 10 > 0)
	{
		for (int i{}; i < Iterations / 10; ++i)
		{
#ifdef CUSTOMECS
			g_ECSUpdateTimes.pop_back();
			g_ECSUpdateTimes.pop_front();
#endif
#ifdef GAMEOBJECT
			g_GOUpdateTimes.pop_back();
			g_GOUpdateTimes.pop_front();
#endif
#ifdef ENTT
			g_enttUpdateTimes.pop_back();
			g_enttUpdateTimes.pop_front();
#endif
		}
	}

	/* Print results */
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