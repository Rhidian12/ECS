#pragma once

/* https://arjunsreedharan.org/ */

namespace ECS
{
	struct alignas(16) BlockInformation final
	{
		size_t BlockSize;
		bool IsFree;
		BlockInformation* pNext;
	};

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
}