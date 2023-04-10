#pragma once

#include <string_view> /* std::string_view */
#include <limits> /* std::numeric_limits */

namespace ECS
{
	namespace Utils
	{
		/* Reference for TypeName: https://stackoverflow.com/questions/35941045/can-i-obtain-c-type-names-in-a-constexpr-way */

		template<typename T>
		[[nodiscard]] constexpr std::string_view ConstexprTypeName();

		template<>
		[[nodiscard]] constexpr std::string_view ConstexprTypeName<void>() { return "void"; }

		/* Should have internal linkage and therefore be unavailable to other files */
		namespace
		{
			template<typename T>
			[[nodiscard]] constexpr static const char* WrappedTypeName()
			{
				return __FUNCSIG__;
			}
		}

		template <typename T>
		[[nodiscard]] constexpr std::string_view ConstexprTypeName()
		{
			constexpr std::string_view wrappedName(WrappedTypeName<T>());

			constexpr size_t endOfType{ wrappedName.find_last_of('>') };
			constexpr size_t beginOfType{ std::max(wrappedName.find_last_of(' '), wrappedName.find_last_of('<')) };

			return wrappedName.substr(beginOfType + 1, endOfType - beginOfType - 1);
		}

		[[nodiscard]] inline constexpr uint32_t ConstexprStringHash(const char* pKey, size_t count)
		{
			uint32_t p = 31;
			const uint32_t m = static_cast<uint32_t>(1e9) + 9;
			uint32_t hash_value = 0;
			uint32_t p_pow = 1;
			const char* c = pKey;
			while (count > 0)
			{
				hash_value = (hash_value + (*c - static_cast<uint32_t>('a') + 1) * p_pow) % m;
				p_pow = (p_pow * p) % m;
				c++;
				--count;
			}

			return hash_value;
		}

		[[nodiscard]] __forceinline float RandomFloat(float min, float max)
		{
			return min + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (max - min)));
		}

		template<typename T>
		[[nodiscard]] __forceinline constexpr bool AreEqual(const T a, const T b, const T epsilon = std::numeric_limits<T>::epsilon())
		{
			static_assert(std::is_fundamental_v<T>, "Utils::AreEqual<T>() > T must be a fundamental type");

			return static_cast<T>(abs(a - b)) <= epsilon;
		}
	}
}