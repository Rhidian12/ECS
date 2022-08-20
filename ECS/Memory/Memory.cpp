#include "Memory.h"

#include <stdlib.h> /* malloc */

namespace ECS
{
	Allocator::Allocator()
		: Allocator{ 1024 }
	{
	}

	Allocator::Allocator(const size_t totalSize)
		: pBuffer{}
		, StackPointer{}
		, Capacity{ RoundToNextPowerOfTwo(totalSize) }
	{
		pBuffer = malloc(Capacity);
	}

	Allocator::~Allocator()
	{
		free(pBuffer);
	}

	void Allocator::Reallocate(const size_t newCapacity)
	{
		free(pBuffer);

		Capacity = RoundToNextPowerOfTwo(newCapacity);
		StackPointer = 0;

		pBuffer = malloc(Capacity);
	}
}