#include "MemoryAllocator.h"

#include <assert.h> /* assert() */
#include <cstdlib> /* malloc */

namespace ECS
{
    void* MemoryAllocator::allocate(size_t size) 
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
        
        if (!pBlock)
        {
            return nullptr;
        }

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