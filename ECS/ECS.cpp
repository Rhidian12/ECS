// ECS.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include "ECSConstants.h"

#include <iostream>

#include "System/System.h"
#include "EntityManager/EntityManager.h"
#include "ComponentManager/ComponentManager.h"
#include "Component/Component.h"
#include "SystemManager/SystemManager.h"

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
			if ((pEntityManager->GetSignatureUnsafe(entity) & flags) != 0)
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

int main(int*, char* [])
{
	using namespace ECS;

	// constexpr int Iterations{ 1'000'000 };
	constexpr int Iterations{ 1 };

	EntityManager* pEntityManager{ new EntityManager{} };
	ComponentManager* pComponentManager{ new ComponentManager{} };
	SystemManager* pSystemManager{ new SystemManager{} };

	GravitySystem* pSystem{ pSystemManager->AddSystem<GravitySystem>() };

	for (int i{}; i < Iterations; ++i)
	{
		Entity entity{ pEntityManager->CreateEntity() };

		pComponentManager->AddComponent<TransformComponent>(entity, new TransformComponent{});
		pComponentManager->AddComponent<RigidBodyComponent>(entity, new RigidBodyComponent{});
		pComponentManager->AddComponent<GravityComponent>(entity, new GravityComponent{});

		pEntityManager->SetSignatureUnsafe(entity, TransformComponent::GetComponentID());
		pEntityManager->SetSignatureUnsafe(entity, RigidBodyComponent::GetComponentID());
		pEntityManager->SetSignatureUnsafe(entity, GravityComponent::GetComponentID());

		pSystem->AddEntity(entity);
	}

	for (int i{}; i < Iterations; ++i)
	{
		pSystem->UpdateSystem();
	}

	delete pEntityManager;
	delete pComponentManager;

	return 0;
}