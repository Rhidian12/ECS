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

__forceinline float RandomFloat(float min, float max)
{
	return min + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (max - min)));
}

struct Point2f
{
	float x, y;

	static Point2f CreateRandomPoint2f(float min, float max)
	{
		return Point2f{ RandomFloat(min, max), RandomFloat(min, max) };
	}
};

class GravityComponent final : public ECS::IComponent<GravityComponent>
{
public:
	float Gravity{ -981.f };
};

class TransformComponent final : public ECS::IComponent<TransformComponent>
{
public:
	Point2f Position{ Point2f::CreateRandomPoint2f(0.f, 1000.f) };
	Point2f Rotation{ Point2f::CreateRandomPoint2f(0.f, 360.f) };
	Point2f Scale{ Point2f::CreateRandomPoint2f(0.f, 3.f) };
};

class RigidBodyComponent final : public ECS::IComponent<RigidBodyComponent>
{
public:
	float Mass{ RandomFloat(0.f, 100.f) };
	Point2f Velocity{};
};

class GOGravityComponent final : public GO::Component
{
public:
	float Gravity{ -981.f };
};

class GORigidBodyComponent final : public GO::Component
{
public:
	GORigidBodyComponent(GOGravityComponent* const pGravityComponent)
		: pGravityComponent{ pGravityComponent }
		, Velocity{}
	{}

	virtual void Update() override
	{
		Velocity.y += pGravityComponent->Gravity * Mass;
	}

	float Mass{ RandomFloat(0.f, 100.f) };
	Point2f Velocity;
	GOGravityComponent* pGravityComponent;
};

class GOTransformComponent final : public GO::Component
{
public:
	GOTransformComponent(GORigidBodyComponent* const pRigidbodyComponent)
		: pRigidbodyComponent{ pRigidbodyComponent }
	{}

	virtual void Update() override
	{
		Position.x += pRigidbodyComponent->Velocity.x;
		Position.y += pRigidbodyComponent->Velocity.y;
	}

	GORigidBodyComponent* pRigidbodyComponent;
	Point2f Position{ Point2f::CreateRandomPoint2f(0.f, 1000.f) };
	Point2f Rotation{ Point2f::CreateRandomPoint2f(0.f, 360.f) };
	Point2f Scale{ Point2f::CreateRandomPoint2f(0.f, 3.f) };
};

struct ENTTGravity final
{
	float Gravity{ -981.f };
};

struct ENTTTransformComponent final
{
	Point2f Position{ Point2f::CreateRandomPoint2f(0.f, 1000.f) };
	Point2f Rotation{ Point2f::CreateRandomPoint2f(0.f, 360.f) };
	Point2f Scale{ Point2f::CreateRandomPoint2f(0.f, 3.f) };
};

struct ENTTRigidBodyComponent final
{
	float Mass{ RandomFloat(0.f, 100.f) };
	Point2f Velocity{};
};

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
// #define GAMEOBJECT
#define CUSTOMECS
// #define ENTT

// #define WRITE_TO_FILE
// #define APPEND_TO_FILE
// #define OVERWRITE_TO_FILE

/* Things to test */
#ifdef CUSTOMECS
ECS::System g_GravitySystem;
#elif defined(GAMEOBJECT)
std::vector<GO::GameObject*> g_GameObjects;
#elif defined(ENTT)
entt::registry g_Registry;
#endif

#ifdef CUSTOMECS
std::deque<long long> g_ECSTimes{};
#elif defined(GAMEOBJECT)
std::deque<long long> g_GOTimes{};
#elif defined(ENTT)
std::deque<long long> g_enttTimes{};
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

	g_ECSTimes.push_back(std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count());
}
#elif defined(GAMEOBJECT)
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

	g_GOTimes.push_back(std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count());
}
#elif defined(ENTT)
void TestENTT()
{
	std::chrono::steady_clock::time_point t1{};
	std::chrono::steady_clock::time_point t2{};

	t1 = std::chrono::steady_clock::now();

	ENTTUpdate(g_Registry);

	t2 = std::chrono::steady_clock::now();

	g_enttTimes.push_back(std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count());
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

#elif defined(GAMEOBJECT)
		GameObject* pG{ new GameObject{} };

		pG->AddComponent(new GOGravityComponent{});
		pG->AddComponent(new GORigidBodyComponent{ pG->GetComponent<GOGravityComponent>() });
		pG->AddComponent(new GOTransformComponent{ pG->GetComponent<GORigidBodyComponent>() });

		g_GameObjects.push_back(pG);

#elif defined(ENTT)
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
#elif defined(GAMEOBJECT)
		TestGO();
#elif defined(ENTT)
		TestENTT();
#endif
	}

	/* Cleanup results */
#ifdef CUSTOMECS
	std::sort(g_ECSTimes.begin(), g_ECSTimes.end());
#elif defined(GAMEOBJECT)
	std::sort(g_GOTimes.begin(), g_GOTimes.end());
#elif defined(ENTT)
	std::sort(g_enttTimes.begin(), g_enttTimes.end());
#endif
	if (Iterations / 10 > 0)
	{
		for (int i{}; i < Iterations / 10; ++i)
		{
#ifdef CUSTOMECS
			g_ECSTimes.pop_back();
			g_ECSTimes.pop_front();

#elif defined(GAMEOBJECT)
			g_GOTimes.pop_back();
			g_GOTimes.pop_front();

#elif defined(ENTT)
			g_enttTimes.pop_back();
			g_enttTimes.pop_front();
#endif
		}
	}

	/* Print results */
#ifdef CUSTOMECS
	std::cout << "ECS Average:\t\t" << std::accumulate(g_ECSTimes.cbegin(), g_ECSTimes.cend(), (long long)0) / g_ECSTimes.size() << " nanoseconds\n";
#elif defined(GAMEOBJECT)
	std::cout << "GO Average:\t\t" << std::accumulate(g_GOTimes.cbegin(), g_GOTimes.cend(), (long long)0) / g_GOTimes.size() << " nanoseconds\n";
#elif defined(ENTT)
	std::cout << "ENTT Average:\t\t" << std::accumulate(g_enttTimes.cbegin(), g_enttTimes.cend(), (long long)0) / g_enttTimes.size() << " nanoseconds\n";
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
		file << "ECS Average: " << std::accumulate(g_ECSTimes.cbegin(), g_ECSTimes.cend(), (long long)0) / g_ECSTimes.size() << " nanoseconds\n\n";
#elif defined(GAMEOBJECT)
		file << "GO Average: " << std::accumulate(g_GOTimes.cbegin(), g_GOTimes.cend(), (long long)0) / g_GOTimes.size() << " nanoseconds\n\n";
#elif defined(ENTT)
		file << "ENTT Average: " << std::accumulate(g_enttTimes.cbegin(), g_enttTimes.cend(), (long long)0) / g_enttTimes.size() << " nanoseconds\n\n";
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