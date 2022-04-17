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

	template<typename Type>
	class PoolAllocator
	{
	public:
		PoolAllocator() = default;

		PoolAllocator(const PoolAllocator&) = delete;
		PoolAllocator(PoolAllocator&&) = delete;
		PoolAllocator& operator=(const PoolAllocator&) = delete;
		PoolAllocator& operator=(PoolAllocator&&) = delete;

		virtual ~PoolAllocator();

		Type* allocate(size_t nrOfElementsToAllocate);

		/* unreferenced size_t for STL */
		void deallocate(void* pBlock);

	private:
		BlockInformation* GetFreeBlock(size_t size) const;

		BlockInformation* Head{};
		BlockInformation* Tail{};
	};

	template<typename Type>
	PoolAllocator<Type>::~PoolAllocator()
	{
		BlockInformation* pTemp{ Head };

		while (pTemp)
		{
			BlockInformation* pNext{ pTemp->pNext };

			free(pTemp->Data);
			free(pTemp);

			pTemp = pNext;
		}
	}

	template<typename Type>
	Type* PoolAllocator<Type>::allocate(size_t nrOfElementsToAllocate)
	{
		assert(nrOfElementsToAllocate != 0);

		BlockInformation* pBlockInfo{ GetFreeBlock(nrOfElementsToAllocate) };

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
	void PoolAllocator<Type>::deallocate(void* pBlock)
	{
		assert(pBlock);

		/* THIS IS EXTREMELY HARDCODED, BUT THIS COULD ONLY BE SOLVED VIA REFLECTION */

		/* Move back 3 addresses for the start of the BlockInformation */
		BlockInformation** pBlockInfo = reinterpret_cast<BlockInformation**>(&pBlock - 3);

		(*pBlockInfo)->IsFree = true;
	}

	template<typename Type>
	BlockInformation* PoolAllocator<Type>::GetFreeBlock(size_t size) const
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