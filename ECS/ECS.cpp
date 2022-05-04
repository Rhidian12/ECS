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

class GravityComponent final : public ECS::Component<GravityComponent>
{
public:
	float Gravity{ -981.f };
};

class TransformComponent final : public ECS::Component<TransformComponent>
{
public:
	Point2f Position{ Point2f::CreateRandomPoint2f(0.f, 1000.f) };
	Point2f Rotation{ Point2f::CreateRandomPoint2f(0.f, 360.f) };
	Point2f Scale{ Point2f::CreateRandomPoint2f(0.f, 3.f) };
};

class RigidBodyComponent final : public ECS::Component<RigidBodyComponent>
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

	view.ForEach([](auto gravity, auto rigidBody, auto transform)->void
		{
			rigidBody.Velocity.y += gravity.Gravity * rigidBody.Mass;

			transform.Position.x += rigidBody.Velocity.x;
			transform.Position.y += rigidBody.Velocity.y;
		});
}

int main(int*, char* [])
{
	using namespace ECS;
	using namespace GO;

	constexpr Entity AmountOfEntities{ MaxEntities };
	constexpr int Iterations{ 100 };

	System gravitySystem{};

	std::vector<GameObject*> GameObjects;

	entt::registry registry;

	std::deque<long long> ECSTimes{};
	std::deque<long long> GOTimes{};
	std::deque<long long> enttTimes{};

	for (int i{}; i < AmountOfEntities; ++i)
	{
		Entity entity{ gravitySystem.CreateEntity() };

		gravitySystem.AddComponent<TransformComponent>(entity);
		gravitySystem.AddComponent<RigidBodyComponent>(entity);
		gravitySystem.AddComponent<GravityComponent>(entity);

		GameObject* pG{ new GameObject{} };

		pG->AddComponent(new GOGravityComponent{});
		pG->AddComponent(new GORigidBodyComponent{ pG->GetComponent<GOGravityComponent>() });
		pG->AddComponent(new GOTransformComponent{ pG->GetComponent<GORigidBodyComponent>() });

		GameObjects.push_back(pG);

		const auto enttEntity{ registry.create() };
		registry.emplace<ENTTGravity>(enttEntity);
		registry.emplace<ENTTTransformComponent>(enttEntity);
		registry.emplace<ENTTRigidBodyComponent>(enttEntity);
	}

	std::chrono::steady_clock::time_point t1{};
	std::chrono::steady_clock::time_point t2{};

	for (int i{}; i < Iterations; ++i)
	{
		t1 = std::chrono::steady_clock::now();

		GravityUpdate(gravitySystem);

		t2 = std::chrono::steady_clock::now();

		ECSTimes.push_back(std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count());

		t1 = std::chrono::steady_clock::now();

		for (GameObject* const pG : GameObjects)
		{
			pG->Update();
		}

		t2 = std::chrono::steady_clock::now();

		GOTimes.push_back(std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count());

		t1 = std::chrono::steady_clock::now();

		ENTTUpdate(registry);

		t2 = std::chrono::steady_clock::now();

		enttTimes.push_back(std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count());
	}

	std::sort(ECSTimes.begin(), ECSTimes.end());
	std::sort(GOTimes.begin(), GOTimes.end());

	if (Iterations / 10 > 0)
	{
		for (int i{}; i < Iterations / 10; ++i)
		{
			ECSTimes.pop_back();
			ECSTimes.pop_front();

			GOTimes.pop_back();
			GOTimes.pop_front();

			enttTimes.pop_back();
			enttTimes.pop_front();
		}
	}

	std::cout << "ECS Average:\t\t" << std::accumulate(ECSTimes.cbegin(), ECSTimes.cend(), (long long)0) / ECSTimes.size() << " nanoseconds\n";
	std::cout << "GO Average:\t\t" << std::accumulate(GOTimes.cbegin(), GOTimes.cend(), (long long)0) / GOTimes.size() << " nanoseconds\n";
	std::cout << "ENTT Average:\t\t" << std::accumulate(enttTimes.cbegin(), enttTimes.cend(), (long long)0) / enttTimes.size() << " nanoseconds\n";

	for (GameObject* pG : GameObjects)
		delete pG;

	return 0;
}