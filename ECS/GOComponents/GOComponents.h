#pragma once

#include "../GOComponent/GOComponent.h"
#include "../Point2f/Point2f.h"
#include "../Utils/Utils.h"

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

	float Mass{ ECS::Utils::RandomFloat(0.f, 100.f) };
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