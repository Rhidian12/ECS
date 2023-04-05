#pragma once
#include "../ECSConstants.h"
#include "../Utils/Utils.h"

#include <string> /* std::string */

namespace ECS
{
	template<typename T>
	consteval ComponentType GenerateComponentID()
	{
		using namespace Utils;

		constexpr std::string_view typeName(ConstexprTypeName<T>());

		constexpr ComponentType hash(
			static_cast<ComponentType>(
				ConstexprStringHash(
					typeName.data(),
					typeName.size()
				))
		);

		return hash;
	}
}