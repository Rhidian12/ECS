#pragma once

#include "../Point2f/Point2f.h"
#include "../Utils/Utils.h"
#include "../TypeList/TypeList.h"

#define PATH "../ECSComponents/ECSComponents.h"
#include "../RegisterComponent/RegisterComponent.h"

namespace ECS
{
	class GravityComponent final
	{
	public:
		float Gravity{ -981.f };
	};

	using TypeListGravity = typename Concatenate<TYPE_LIST, GravityComponent>::Type;
#undef TYPE_LIST
#define TYPE_LIST TypeListGravity

	class TransformComponent final
	{
	public:
		Point2f Position{ Point2f::CreateRandomPoint2f(0.f, 1000.f) };
		Point2f Rotation{ Point2f::CreateRandomPoint2f(0.f, 360.f) };
		Point2f Scale{ Point2f::CreateRandomPoint2f(0.f, 3.f) };
	};

	using TypeListTransform = typename Concatenate<TYPE_LIST, TransformComponent>::Type;
#undef TYPE_LIST
#define TYPE_LIST TypeListTransform

	class RigidBodyComponent final
	{
	public:
		float Mass{ ECS::Utils::RandomFloat(0.f, 100.f) };
		Point2f Velocity{};
	};

	using TypeListRigidBody = typename Concatenate<TYPE_LIST, RigidBodyComponent>::Type;
#undef TYPE_LIST
#define TYPE_LIST TypeListRigidBody
}