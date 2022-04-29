#include "PoolAllocator.h"

namespace ECS
{
	PoolAllocator::~PoolAllocator()
	{
		for (const auto& list : BlockLists)
		{
			for (void* const pBlock : list)
			{
				free(pBlock);
			}
		}
	}

	BlockInformation* PoolAllocator::GetFreeBlock(size_t bucket)
	{
		if (!FreeBlockLists[bucket].empty())
		{
			BlockInformation* pBlock{ static_cast<BlockInformation*>(FreeBlockLists[bucket].front()) };
			FreeBlockLists[bucket].pop();

			return pBlock;
		}

		return nullptr;
	}
}