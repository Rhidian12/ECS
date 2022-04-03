#pragma once
#include "../ECSConstants.h"

#include <unordered_set>

namespace ECS
{
	class ISystem
	{
	public:
		virtual ~ISystem() = default;
	};

	class System
	{
	public:
		virtual ~System() = default;

		void AddEntity(const Entity id) noexcept;
		void RemoveEntity(const Entity id) noexcept;

		virtual void UpdateSystem() = 0; /* This function will never be called from a System*, it just enforces inheritance */

	protected:
		std::unordered_set<Entity> Entities;
	};
}