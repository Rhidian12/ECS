#pragma once

#include <assert.h> /* assert() */
#include <cstdlib> /* malloc() */

/* Based on the following article: */
/* https://arjunsreedharan.org/ */

namespace ECS
{
	struct alignas(16) BlockInformation final
	{
		size_t BlockSize;
		bool IsFree;
		BlockInformation* pNext;
	};

	template<typename Type>
	class MemoryAllocator final
	{
	public:
		~MemoryAllocator();

		void* allocate(size_t size);
		void deallocate(void* pBlock);

	private:
		BlockInformation* GetFreeBlock(size_t size) const;

		BlockInformation* Head;
		BlockInformation* Tail;
	};

	template<typename Type>
	MemoryAllocator<Type>::~MemoryAllocator()
	{
		BlockInformation* pTemp{ Head };

		while (pTemp)
		{
			BlockInformation* pNext{ pTemp->pNext };

			free(pTemp);

			pTemp = pNext;
		}
	}

	template<typename Type>
	void* MemoryAllocator<Type>::allocate(size_t size)
	{
		assert(size != 0);

		BlockInformation* pBlockInfo{ GetFreeBlock(size) };

		if (pBlockInfo)
		{
			pBlockInfo->IsFree = false;
			return static_cast<void*>(pBlockInfo + 1);
		}

		const size_t totalSize{ sizeof(BlockInformation) + size };
		void* const pBlock{ malloc(totalSize) };

		assert(pBlock);

		pBlockInfo = static_cast<BlockInformation*>(pBlock);
		pBlockInfo->BlockSize = size;
		pBlockInfo->IsFree = false;
		pBlockInfo->pNext = nullptr;

		if (!Head)
		{
			Head = pBlockInfo;
		}

		if (Tail)
		{
			Tail->pNext = pBlockInfo;
		}

		Tail = pBlockInfo;

		return static_cast<void*>(pBlockInfo + 1);
	}
	
	template<typename Type>
	void MemoryAllocator<Type>::deallocate(void* pBlock)
	{
		assert(pBlock);

		BlockInformation* pBlockInfo{};
		BlockInformation* pTemp{};

		pBlockInfo = static_cast<BlockInformation*>(pBlock) - 1;

		pBlockInfo->IsFree = true;
	}

	template<typename Type>
	BlockInformation* MemoryAllocator<Type>::GetFreeBlock(size_t size) const
	{
		BlockInformation* pCurrent{ Head };

		while (pCurrent)
		{
			if (pCurrent->IsFree && pCurrent->BlockSize >= size)
			{
				return pCurrent;
			}

			pCurrent = pCurrent->pNext;
		}

		return nullptr;
	}
}