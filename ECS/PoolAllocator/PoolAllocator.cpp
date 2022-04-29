#include "PoolAllocator.h"

namespace ECS
{
	void PoolAllocator::ReleaseAllMemory() noexcept
	{
		for (const auto& list : BlockLists)
		{
			for (void* const pBlock : list)
			{
				free(pBlock);
			}
		}

		for (auto& list : FreeBlockLists)
		{
			while (!list.empty())
			{
				free(list.front());
				list.pop();
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