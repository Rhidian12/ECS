#pragma once
#include "../ECSConstants.h"

#include <unordered_set>

namespace ECS
{
	class System
	{
	public:
		virtual ~System() = default;

		virtual void UpdateSystem() = 0;

	protected:
		std::unordered_set<Entity> Entities;
	};
}