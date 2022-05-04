#pragma once

#include <string_view> /* std::string_view */

namespace ECS
{
	namespace Utils
	{
		/* Reference for TypeName: https://stackoverflow.com/questions/35941045/can-i-obtain-c-type-names-in-a-constexpr-way */

		template<typename Type>
		constexpr std::string_view ConstexprTypeName();

		template<>
		constexpr std::string_view ConstexprTypeName<void>() { return "void"; }

		/* Should have internal linkage and therefore be unavailable to other files */
		namespace
		{
			template<typename Type>
			constexpr std::string_view WrappedTypeName()
			{
				return __FUNCSIG__;
			}
		}

		template <typename Type>
		constexpr std::string_view ConstexprTypeName()
		{
			constexpr std::string_view wrappedName(WrappedTypeName<Type>());
			constexpr std::string_view wrappedVoidName(WrappedTypeName<void>());
			constexpr std::string_view voidName(ConstexprTypeName<void>());

			constexpr size_t prefixLength(wrappedVoidName.find(voidName));
			constexpr size_t suffixLength(wrappedVoidName.length() - prefixLength - voidName.length());

			constexpr size_t typeNameLength(wrappedVoidName.length() - prefixLength - suffixLength);
			return wrappedName.substr(prefixLength, typeNameLength);
		}
	}
}