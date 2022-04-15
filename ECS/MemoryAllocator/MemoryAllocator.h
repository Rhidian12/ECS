#pragma once

#include <assert.h>
#include <limits>

namespace ECS
{
	template<typename Type>
	struct MemoryBlock final
	{
		size_t BlockSize;
		bool IsUsed;

		Type Data[1];
	};

	template<typename Type>
	class MemoryAllocator final
	{
	private:
		inline static constexpr SizeOfType = sizeof(Type);

	public:
		using value_type = Type;

		[[nodiscard]] Type* allocate(size_t sizeToAllocate);

		void deallocate(Type* pMemoryToFree);
	};

	template<typename Type>
	Type* MemoryAllocator<Type>::allocate(size_t sizeToAllocate)
	{
		assert(sizeToAllocate <= (std::numeric_limits<size_t>::max() / SizeOfType));

		return static_cast<Type*>(malloc(sizeToAllocate * SizeOfType));
	}

	template<typename Type>
	void MemoryAllocator<Type>::deallocate(Type* pMemoryToFree)
	{
		free(pMemoryToFree);
	}

	template<typename Type, typename OtherType>
	constexpr bool operator==(const MemoryAllocator<Type>&, const MemoryAllocator<OtherType>&)
	{
		if constexpr (std::is_same_v<Type, OtherType>)
		{
			return true;
		}

		return false;
	}

	template<typename Type, typename OtherType>
	constexpr bool operator!=(const MemoryAllocator<Type>& a, const MemoryAllocator<OtherType>& b)
	{
		return !(a == b);
	}
}