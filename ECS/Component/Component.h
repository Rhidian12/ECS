#pragma once
#include "../ECSConstants.h"

namespace ECS
{
	class Component
	{
	public:
		virtual ~Component() = default;

		__forceinline auto GetComponentID() const noexcept { return ComponentID; }

	protected:
		/* we need a way to differentiate components from one another, and I refuse to use typeid() [cus it's fucking cringe] */
		ComponentType ComponentID;
	};
}