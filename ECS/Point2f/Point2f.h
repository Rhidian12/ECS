#pragma once

#include "../Utils/Utils.h"

struct Point2f
{
	float x, y;

	static Point2f CreateRandomPoint2f(float min, float max)
	{
		return Point2f{ ECS::Utils::RandomFloat(min, max), ECS::Utils::RandomFloat(min, max) };
	}
};