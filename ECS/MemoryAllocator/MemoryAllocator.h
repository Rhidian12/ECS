#pragma once

#include <assert.h> /* assert() */
#include <limits> /* std::numeric_limits */
#include <utility> /* std::declval */

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
		inline static constexpr size_t SizeOfBlock = sizeof(MemoryBlock);
		inline static constexpr size_t SizeOfBlockData = sizeof(std::declval<MemoryBlock>().Data);

	public:
		using value_type = Type;

		[[nodiscard]] Type* allocate(size_t sizeToAllocate);

		void deallocate(Type* pMemoryToFree);

	private:
		MemoryBlock* const AllocateMemory(size_t sizeToAllocate);
		size_t AlignBlock(size_t blockSize);
	};

	template<typename Type>
	Type* MemoryAllocator<Type>::allocate(size_t sizeToAllocate)
	{
		assert(sizeToAllocate <= (std::numeric_limits<size_t>::max() / SizeOfType));

		MemoryBlock* const pBlock{ AllocateMemory(sizeToAllocate) };
	
		pBlock->IsUsed = true;
		pBlock->BlockSize = sizeToAllocate;

		return pBlock->Data;
	}

	template<typename Type>
	void MemoryAllocator<Type>::deallocate(Type* pMemoryToFree)
	{
		free(pMemoryToFree);
	}

	template<typename Type>
	MemoryBlock* const MemoryAllocator<Type>::AllocateMemory(size_t sizeToAllocate)
	{
		/* Make sure that the block we're requesting is aligned to a Word */
		sizeToAllocate = AlignBlock(sizeToAllocate);

		/* Increase the size to keep in mind the memory block struct - the data we already allocate inside the block */
		sizeToAllocate += SizeOfBlock - SizeOfBlockData;

		return static_cast<MemoryBlock*>(malloc(sizeToAllocate));
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