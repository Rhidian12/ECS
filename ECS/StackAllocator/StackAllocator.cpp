//#include "StackAllocator.h"
//#include <stdlib.h>
//
//namespace ECS
//{
//	namespace Memory
//	{
//		LinkedListAllocator::LinkedListAllocator(size_t startSize)
//			: pBuffer{}
//			, Size{ startSize }
//			, StackPointer{}
//		{
//			pBuffer = static_cast<char*>(malloc(AlignToSize(startSize)));
//		}
//
//		LinkedListAllocator::~LinkedListAllocator()
//		{
//			free(pBuffer);
//		}
//	}
//}