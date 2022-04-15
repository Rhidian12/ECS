#pragma once

#include <assert.h>
#include <limits>

namespace ECS
{
	/* Machine Word size. This is either 8 bytes or 4 bytes depending on architecture (x64 vs x86) */
	using Word = intptr_t;
	inline static constexpr Word WordSize{ sizeof(Word) };

	struct MemoryBlock final
	{
		size_t BlockSize;
		bool IsUsed;

		Word Data[1];
	};

	template<typename Type>
	class MemoryAllocator final
	{
	private:
		inline static constexpr size_t SizeOfType = sizeof(Type);

	public:
		using value_type = Type;

		[[nodiscard]] Type* allocate(size_t sizeToAllocate);

		void deallocate(Type* pMemoryToFree);

	private:
		size_t AlignBlock(size_t blockSize);
	};

	template<typename Type>
	Type* MemoryAllocator<Type>::allocate(size_t sizeToAllocate)
	{
		assert(sizeToAllocate <= (std::numeric_limits<size_t>::max() / SizeOfType));

		/* Make sure that the block we're requesting is aligned to a Word */
		sizeToAllocate = AlignBlock(sizeToAllocate);
		
		return static_cast<Type*>(malloc(sizeToAllocate * SizeOfType));
	}

	template<typename Type>
	void MemoryAllocator<Type>::deallocate(Type* pMemoryToFree)
	{
		free(pMemoryToFree);
	}

	template<typename Type>
	size_t MemoryAllocator<Type>::AlignBlock(size_t blockSize)
	{
		/* Aligns everything to WordSize multiples */
		return (blockSize + WordSize - 1) & ~(WordSize - 1);
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