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
			using TypeNameProber = void;

			template<typename Type>
			constexpr std::string_view WrappedTypeName()
			{
#if defined(__clang__) || defined(__GNUC__)
				return __PRETTY_FUNCTION__;
#elif defined(_MSC_VER)
				return __FUNCSIG__;
#else
#error "Unsupported Compiler!"
#endif
			}

			constexpr size_t WrappedTypeNamePrefixLength()
			{
				return WrappedTypeName<TypeNameProber>().find(ConstexprTypeName<TypeNameProber>());
			}

			constexpr size_t WrappedTypeNameSuffixLength()
			{
				return WrappedTypeName<TypeNameProber>().length() -
					WrappedTypeNamePrefixLength() -
					ConstexprTypeName<TypeNameProber>().length();
			}
		}

		template <typename Type>
		constexpr std::string_view ConstexprTypeName()
		{
			constexpr std::string_view wrappedName(WrappedTypeName<Type>());
			constexpr size_t prefixLength(WrappedTypeNamePrefixLength());
			constexpr size_t suffixLength(WrappedTypeNameSuffixLength());
			constexpr size_t typeNameLength(wrappedName.length() - prefixLength - suffixLength);
			return wrappedName.substr(prefixLength, typeNameLength);
		}
	}
}