#pragma once

#include "../ECSConstants.h"

namespace ECS
{
	class IComponentArray
	{
	public:
		virtual ~IComponentArray() = default;

		virtual void Remove(const Entity entity) = 0;
	};

	class ComponentArray final
	{
	};
}