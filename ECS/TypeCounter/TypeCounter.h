#pragma once
#include "../ECSConstants.h"
#include "../Utils/Utils.h"

namespace ECS
{
	template<typename Type>
	constexpr ComponentType GenerateID()
	{
		using namespace Utils;

		constexpr const char* typeName(ConstexprTypeName<Type>());

		constexpr ComponentType hash(static_cast<ComponentType>(CONSTEXPRSTRINGHASH(typeName)));

		return hash;
	}
}