#include "Memory.h"

#include <stdlib.h> /* malloc */

namespace ECS
{
	StackAllocator::StackAllocator()
		: StackAllocator{ 1024 }
	{
	}

	StackAllocator::StackAllocator(const size_t totalSize)
		: pBuffer{}
		, StackPointer{}
		, Capacity{ RoundToNextPowerOfTwo(totalSize) }
	{
		pBuffer = malloc(Capacity);
	}

	StackAllocator::StackAllocator(StackAllocator&& other) noexcept
		: pBuffer{ std::move(other.pBuffer) }
		, StackPointer{ std::move(other.StackPointer) }
		, Capacity{ std::move(other.Capacity) }
	{
		other.pBuffer = nullptr;
	}

	StackAllocator& StackAllocator::operator=(StackAllocator&& other) noexcept
	{
		pBuffer = std::move(other.pBuffer);
		StackPointer = std::move(other.StackPointer);
		Capacity = std::move(other.Capacity);

		other.pBuffer = nullptr;

		return *this;
	}

	StackAllocator::~StackAllocator()
	{
		free(pBuffer);
	}

	void StackAllocator::Reallocate(const size_t newCapacity)
	{
		free(pBuffer);

		Capacity = RoundToNextPowerOfTwo(newCapacity);
		StackPointer = 0;

		pBuffer = malloc(Capacity);
	}
}