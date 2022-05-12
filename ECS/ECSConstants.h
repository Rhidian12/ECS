#pragma once
#include <cstdint> /* uint64_t */
#include <limits> /* std::numeric_limits */
#include <bitset> /* std::bitset */

namespace ECS
{
	using Entity = uint16_t; /* This supports ~65k possible entities, seems like enough */
	using ComponentType = uint8_t; /* This supports 65,535 possible component types, seems like enough */
	using SystemID = ComponentType; /* This supports as many systems as there are Components */

	constexpr Entity MaxEntities{ std::numeric_limits<Entity>::max() - 1 };
	constexpr ComponentType MaxComponentTypes{ std::numeric_limits<ComponentType>::max() -1 };
	constexpr SystemID MaxSystems{ MaxComponentTypes };

	using EntitySignature = std::bitset<MaxComponentTypes>;

	constexpr Entity InvalidEntityID{ std::numeric_limits<Entity>::max() };
	constexpr ComponentType InvalidComponentID{ std::numeric_limits<ComponentType>::max() };
	constexpr SystemID InvalidSystemID{ InvalidComponentID };
}