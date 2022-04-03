// ECS.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include "ECSConstants.h"

#include <iostream>

#include "System/System.h"
#include "EntityManager/EntityManager.h"
#include "ComponentManager/ComponentManager.h"
#include "Component/Component.h"
#include "SystemManager/SystemManager.h"

#include "GOComponent/GOComponent.h"
#include "GameObject/GameObject.h"

#include <chrono>
#include <deque>
#include <algorithm>
#include <numeric>
#include <vector>

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
	Point2f Velocity;
};

class GravitySystem final : public ECS::System<GravitySystem>
{
public:
	virtual void UpdateSystem() override
	{
		using namespace ECS;

		if (flags.none())
		{
			flags.set(TransformComponent::GetComponentID());
			flags.set(GravityComponent::GetComponentID());
			flags.set(RigidBodyComponent::GetComponentID());
		}

		for (const Entity& entity : Entities)
		{
			if ((pEntityManager->GetEntitySignatureSafe(entity) & flags) != 0)
			{
				RigidBodyComponent* const rigidBody = pComponentManager->GetComponent<RigidBodyComponent>(entity);
				TransformComponent* const transform = pComponentManager->GetComponent<TransformComponent>(entity);
				const GravityComponent* const gravity = pComponentManager->GetComponent<GravityComponent>(entity);

				rigidBody->Velocity.y += gravity->Gravity * rigidBody->Mass;

				transform->Position.x += rigidBody->Velocity.x;
				transform->Position.y += rigidBody->Velocity.y;
			}
		}
	}

	inline static std::bitset<ECS::MaxComponentTypes> flags{};
	ECS::EntityManager* pEntityManager;
	ECS::ComponentManager* pComponentManager;
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

int main(int*, char* [])
{
	using namespace ECS;
	using namespace GO;

	constexpr int Iterations{ ECS::MaxEntities };

	EntityManager* pEntityManager{ new EntityManager{} };
	ComponentManager* pComponentManager{ new ComponentManager{} };
	SystemManager* pSystemManager{ new SystemManager{} };

	GravitySystem* pSystem{ pSystemManager->AddSystem<GravitySystem>() };

	pSystem->pEntityManager = pEntityManager;
	pSystem->pComponentManager = pComponentManager;

	std::vector<GameObject*> GameObjects;

	std::deque<long long> ECSTimes{};
	std::deque<long long> GOTimes{};

	for (int i{}; i < Iterations; ++i)
	{
		Entity entity{ pEntityManager->CreateEntity() };

		pComponentManager->AddComponent<TransformComponent>(entity, new TransformComponent{});
		pComponentManager->AddComponent<RigidBodyComponent>(entity, new RigidBodyComponent{});
		pComponentManager->AddComponent<GravityComponent>(entity, new GravityComponent{});

		pEntityManager->SetSignatureSafe(entity, TransformComponent::GetComponentID());
		pEntityManager->SetSignatureSafe(entity, RigidBodyComponent::GetComponentID());
		pEntityManager->SetSignatureSafe(entity, GravityComponent::GetComponentID());

		pSystem->AddEntity(entity);

		GameObject* pG{ new GameObject{} };

		pG->AddComponent(new GOGravityComponent{});
		pG->AddComponent(new GORigidBodyComponent{ pG->GetComponent<GOGravityComponent>() });
		pG->AddComponent(new GOTransformComponent{ pG->GetComponent<GORigidBodyComponent>() });

		GameObjects.push_back(pG);
	}


	for (int i{}; i < Iterations; ++i)
	{
		std::chrono::steady_clock::time_point t1{ std::chrono::steady_clock::now() };

		pSystem->UpdateSystem();

		std::chrono::steady_clock::time_point t2{ std::chrono::steady_clock::now() };

		ECSTimes.push_back(std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count());
	}

	for (int i{}; i < Iterations; ++i)
	{
		std::chrono::steady_clock::time_point t1{ std::chrono::steady_clock::now() };

		for (GameObject* const pG : GameObjects)
		{
			pG->Update();
		}

		std::chrono::steady_clock::time_point t2{ std::chrono::steady_clock::now() };

		GOTimes.push_back(std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count());
	}

	std::sort(ECSTimes.begin(), ECSTimes.end());
	std::sort(GOTimes.begin(), GOTimes.end());

	for (int i{}; i < Iterations / 10; ++i)
	{
		ECSTimes.pop_back();
		ECSTimes.pop_front();

		GOTimes.pop_back();
		GOTimes.pop_front();
	}

	std::cout << "ECS Average: " << std::accumulate(ECSTimes.begin(), ECSTimes.end(), 0.f) << "\n";
	std::cout << "GO Average: " << std::accumulate(GOTimes.begin(), GOTimes.end(), 0.f) << "\n";

	delete pEntityManager;
	delete pComponentManager;

	for (GameObject* pG : GameObjects)
		delete pG;

	return 0;
}