#pragma once

#include "../Point2f/Point2f.h"
#include "../Utils/Utils.h"

class GravityComponent final
{
public:
	float Gravity{ -981.f };
};

class TransformComponent final
{
public:
	Point2f Position{ CreateRandomPoint2f(0.f, 1000.f) };
	Point2f Rotation{ CreateRandomPoint2f(0.f, 360.f) };
	Point2f Scale{ CreateRandomPoint2f(0.f, 3.f) };
};

class RigidBodyComponent final
{
public:
	float Mass{ ECS::Utils::RandomFloat(0.f, 100.f) };
	Point2f Velocity{};
};
