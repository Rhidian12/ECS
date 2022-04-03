#pragma once
#include <cstdint> /* uint64_t */
#include <limits> /* std::numeric_limits */
#include <bitset> /* std::bitset */

namespace ECS
{
	using Entity = uint32_t; /* This supports 4,294,967,295 possible entities, seems like enough */
	using ComponentType = uint16_t; /* This supports 65,535 possible component types, seems like enough */
	using SystemID = ComponentType; /* This supports as many systems as there are Components */

// #undef max /* This always causes issues with anythin std::<>::max() related */
	constexpr ComponentType MaxComponentTypes{ std::numeric_limits<ComponentType>::max() };
	constexpr Entity MaxEntities{ std::numeric_limits<Entity>::max() };
	constexpr SystemID MaxSystems{ MaxComponentTypes };
// #define max(a,b) (((a) > (b)) ? (a) : (b))

	using EntitySignature = std::bitset<MaxComponentTypes>;
}