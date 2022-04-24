#pragma once

#include <assert.h> /* assert() */
#include <cstdlib> /* malloc() */
#include <type_traits> /* std::type_traits */

/* Based on the following article: */
/* https://arjunsreedharan.org/ */

namespace ECS
{
	inline static constexpr size_t AlignSize{ 8 };

	/* MOVING THE VARIABLES INSIDE THIS STRUCT WILL BREAK THE ALLOCATOR, SEE PoolAllocator::deallocate() !!! */
	struct alignas(AlignSize) BlockInformation final
	{
		size_t BlockSize;
		bool IsFree;

		BlockInformation* pNext;
		void* Data;
	};

	class PoolAllocator final
	{
	public:
		PoolAllocator(const PoolAllocator& other) noexcept = delete;
		PoolAllocator(PoolAllocator&& other) noexcept = delete;
		PoolAllocator& operator=(const PoolAllocator& other) noexcept = delete;
		PoolAllocator& operator=(PoolAllocator&& other) noexcept = delete;

		template<typename Type>
		static Type* allocate(size_t nrOfElementsToAllocate);

		static void deallocate(void* pBlock);

		static void ReleaseAllMemory() noexcept;

	private:
		template<typename Type>
		static BlockInformation* GetFreeBlock(size_t size);

		inline static BlockInformation* Head{ nullptr };
		inline static BlockInformation* Tail{ nullptr };
	};

	template<typename Type>
	Type* PoolAllocator::allocate(size_t nrOfElementsToAllocate)
	{
		assert(nrOfElementsToAllocate != 0);

		BlockInformation* pBlockInfo{ GetFreeBlock<Type>(nrOfElementsToAllocate) };

		if (pBlockInfo)
		{
			pBlockInfo->IsFree = false;
			return reinterpret_cast<Type*>(pBlockInfo->Data);
		}

		const size_t totalSize{ sizeof(BlockInformation) + nrOfElementsToAllocate * sizeof(Type) };
		void* const pBlock{ malloc(totalSize) };

		assert(pBlock);

		pBlockInfo = static_cast<BlockInformation*>(pBlock);
		pBlockInfo->BlockSize = nrOfElementsToAllocate * sizeof(Type);
		pBlockInfo->IsFree = false;
		pBlockInfo->pNext = nullptr;
		pBlockInfo->Data = malloc(nrOfElementsToAllocate * sizeof(Type));

		if (!Head)
		{
			Head = pBlockInfo;
		}

		if (Tail)
		{
			Tail->pNext = pBlockInfo;
		}

		Tail = pBlockInfo;

		return reinterpret_cast<Type*>(pBlockInfo->Data);
	}

	template<typename Type>
	BlockInformation* PoolAllocator::GetFreeBlock(size_t size)
	{
		BlockInformation* pCurrent{ Head };

		while (pCurrent)
		{
			if (pCurrent->IsFree && pCurrent->BlockSize >= size * sizeof(Type))
			{
				return pCurrent;
			}

			pCurrent = pCurrent->pNext;
		}

		return nullptr;
	}
}