#include "PoolAllocator.h"

namespace ECS
{
	//PoolAllocator::~PoolAllocator() noexcept
	//{
	//	BlockInformation* pTemp{ Head };

	//	while (pTemp)
	//	{
	//		BlockInformation* pNext{ pTemp->pNext };

	//		free(pTemp->Data);
	//		free(pTemp);

	//		pTemp = pNext;
	//	}
	//}

	void PoolAllocator::deallocate(void* pBlock)
	{
		assert(pBlock);

		/* THIS IS EXTREMELY HARDCODED, BUT THIS COULD ONLY BE SOLVED VIA REFLECTION */

		/* Move back 3 addresses for the start of the BlockInformation */
		BlockInformation** pBlockInfo = reinterpret_cast<BlockInformation**>(&pBlock - 3);

		(*pBlockInfo)->IsFree = true;
	}

	void PoolAllocator::ReleaseAllMemory() noexcept
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
}