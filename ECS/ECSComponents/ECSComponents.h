#pragma once

#include "../Component/Component.h"

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
