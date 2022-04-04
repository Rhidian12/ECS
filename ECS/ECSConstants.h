#pragma once
#include <cstdint> /* uint64_t */
#include <limits> /* std::numeric_limits */
#include <bitset> /* std::bitset */

namespace ECS
{
	// using Entity = uint32_t; /* This supports 4,294,967,295 possible entities, seems like enough */
	using Entity = uint32_t; /* This supports 65,535 possible entities, seems like enough */
	using ComponentType = uint8_t; /* This supports 65,535 possible component types, seems like enough */
	using SystemID = ComponentType; /* This supports as many systems as there are Components */

// #undef max /* This always causes issues with anythin std::<>::max() related */
	constexpr Entity MaxEntities{ std::numeric_limits<Entity>::max() - 1 };
	constexpr ComponentType MaxComponentTypes{ std::numeric_limits<ComponentType>::max() -1 };
	constexpr SystemID MaxSystems{ MaxComponentTypes };
// #define max(a,b) (((a) > (b)) ? (a) : (b))

	using EntitySignature = std::bitset<MaxComponentTypes>;

	constexpr Entity InvalidEntityID{ std::numeric_limits<Entity>::max() };
	constexpr ComponentType InvalidComponentID{ std::numeric_limits<ComponentType>::max() };
	constexpr SystemID InvalidSystemID{ InvalidComponentID };
}