#pragma once

#include <assert.h>
#include <limits>

namespace ECS
{
	template<typename Type>
	struct MemoryAllocator final
	{
		using value_type = Type;

		inline static constexpr SizeOfType = sizeof(Type);

		[[nodiscard]] Type* allocate(size_t sizeToAllocate);
	};

	template<typename Type>
	Type* MemoryAllocator<Type>::allocate(size_t sizeToAllocate)
	{
		assert(sizeToAllocate <= (std::numeric_limits<size_t>::max() / SizeOfType));

		return static_cast<Type*>(malloc(sizeToAllocate * SizeOfType));
	}

}