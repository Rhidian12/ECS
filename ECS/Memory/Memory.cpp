#include "Memory.h"

#include <stdlib.h> /* malloc */

ECS::Allocator::Allocator()
	: Allocator{ 1024 }
{
}

ECS::Allocator::Allocator(const size_t totalSize)
	: pBuffer{}
	, StackPointer{}
{
	pBuffer = malloc(RoundToNextPowerOfTwo(totalSize));
}

ECS::Allocator::~Allocator()
{
	free(pBuffer);
}
