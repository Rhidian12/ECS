#pragma once

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
		void* allocate(size_t size);

	private:
		BlockInformation* GetFreeBlock(size_t size) const;

		BlockInformation* Head;
		BlockInformation* Tail;
	};
}