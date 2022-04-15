#include "MemoryAllocator.h"

#include <assert.h>

namespace ECS
{
    void* MemoryAllocator::allocate(size_t size) 
    {
        assert(size != 0);


    }

    BlockInformation* MemoryAllocator::GetFreeBlock(size_t size) const
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