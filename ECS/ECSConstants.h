#pragma once

#include <bitset> 
#include <cstdint> /* uint64_t */
#include <limits> /* std::numeric_limits */

namespace ECS
{
	using Entity = uint32_t; /* This supports ~4 billion possible entities, seems like enough */
	using ComponentType = uint8_t; /* This supports 256 possible component types, seems like enough */

	constexpr Entity MaxEntities{ std::numeric_limits<Entity>::max() - 1 };
	constexpr ComponentType MaxComponentTypes{ std::numeric_limits<ComponentType>::max() -1 };

	using EntitySignature = std::bitset<MaxComponentTypes>;

	constexpr Entity InvalidEntityID{ std::numeric_limits<Entity>::max() };
	constexpr ComponentType InvalidComponentID{ std::numeric_limits<ComponentType>::max() };
}