//#pragma once
//
//#include <cstddef> /* std::byte */
//#include <iterator> /* std::distance */
//#include <assert.h> /* assert() */
//#include <queue> /* std::queue */
//
//namespace ECS
//{
//	namespace Memory
//	{
//		constexpr size_t AlignSize{ 8 };
//
//		struct MemoryHeader final
//		{
//			bool InUse;
//			size_t Size;
//			MemoryHeader* pNext;
//		};
//
//		struct MemoryBlock final
//		{
//			MemoryHeader Header;
//			char* pData;
//		};
//
//		constexpr size_t MemoryBlockDataOffset{ sizeof(MemoryHeader) };
//
//		class LinkedListAllocator final
//		{
//		public:
//			explicit LinkedListAllocator(size_t startSize = 500);
//			~LinkedListAllocator();
//
//			LinkedListAllocator(const LinkedListAllocator&) noexcept = delete;
//			LinkedListAllocator(LinkedListAllocator&&) noexcept = delete;
//			LinkedListAllocator& operator=(const LinkedListAllocator&) noexcept = delete;
//			LinkedListAllocator& operator=(LinkedListAllocator&&) noexcept = delete;
//
//			template<typename Type>
//			Type* allocate()
//			{
//				constexpr size_t sizeOfType(AlignToSize(sizeof(Type)));
//
//				if (!FreeBlocks.empty())
//				{
//					MemoryBlock* pFreeBlock{ FreeBlocks.front() };
//					FreeBlocks.pop();
//
//					return new (pFreeBlock->pData) Type();
//				}
//			}
//
//			template<typename Type>
//			void deallocate(Type* pData)
//			{
//				const size_t distance(std::distance(pBuffer, pData));
//
//				if (distance == StackPointer)
//				{
//					StackPointer -= AlignToSize(sizeof(Type));
//				}
//			}
//
//			template<typename Type>
//			void destroy(Type* p)
//			{
//				if constexpr (!std::is_trivially_destructible_v<Type>)
//				{
//					if (p)
//					{
//						p->~Type();
//					}
//				}
//			}
//
//			template<typename Type, typename ... Values>
//			void construct(Type* p, const Values&... values)
//			{
//				new (p) Type(std::forward<Values>(values)...);
//			}
//
//		private:
//			constexpr size_t AlignToSize(size_t bytes) const { return (bytes + AlignSize - 1) & ~(AlignSize - 1); }
//
//			MemoryBlock* Buffer;
//			std::queue<MemoryBlock*> FreeBlocks;
//		};
//	}
//}