#pragma once

struct Point2f
{
	float x, y;

	static Point2f CreateRandomPoint2f(float min, float max)
	{
		return Point2f{ RandomFloat(min, max), RandomFloat(min, max) };
	}
};