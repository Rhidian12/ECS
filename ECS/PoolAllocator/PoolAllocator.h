//#pragma once
//
//#include <assert.h> /* assert() */
//#include <cstdlib> /* malloc() */
//#include <type_traits> /* std::type_traits */
//#include <vector>
//#include <queue>
//
///* Based on the following article: */
///* https://arjunsreedharan.org/ */
//
//namespace ECS
//{
//	struct MemoryHeader final
//	{
//		size_t BlockSize;
//	};
//
//	template<typename Size>
//	struct MemoryBlock final
//	{
//		MemoryHeader MemoryHeader;
//		std::byte[Size] Data;
//	};
//
//	class PoolAllocator final
//	{
//	public:
//		PoolAllocator() = default;
//		~PoolAllocator();
//
//		PoolAllocator(const PoolAllocator&) noexcept = delete;
//		PoolAllocator(PoolAllocator&&) noexcept = delete;
//		PoolAllocator& operator=(const PoolAllocator&) noexcept = delete;
//		PoolAllocator& operator=(PoolAllocator&&) noexcept = delete;
//
//		template<typename Type>
//		static Type* allocate(const size_t nrOfElementsToAllocate)
//		{
//			assert(nrOfElementsToAllocate != 0);
//
//			const size_t nrOfBytes{ ((nrOfElementsToAllocate * sizeof(Type) + AlignSize - 1) & ~(AlignSize - 1)) };
//			size_t bucket{ nrOfBytes / sizeof(void*) - 1 };
//
//			assert(nrOfBytes % AlignSize == 0);
//
//			/* Anything bigger than 128 bytes gets stored in the final list */
//			if (bucket >= AmountOfLists)
//			{
//				bucket = AmountOfLists - 1;
//			}
//
//			BlockInformation* pBlockInfo{ GetFreeBlock(bucket) };
//
//			if (pBlockInfo)
//			{
//				pBlockInfo->BlockSize = nrOfBytes;
//				destroy(pBlockInfo->Data);
//
//				return new (&pBlockInfo->Data) Type();
//			}
//
//			void* const pBlock{ malloc(sizeof(BlockInformation) + nrOfBytes) };
//
//			assert(pBlock);
//
//			pBlockInfo = static_cast<BlockInformation*>(pBlock);
//			pBlockInfo->BlockSize = nrOfBytes;
//			pBlockInfo->Data = nullptr;
//			Type* pData = new (&pBlockInfo->Data) Type();
//
//			BlockLists[bucket].push_back(pBlockInfo);
//
//			return pData;
//		}
//
//		template<typename Type>
//		static void deallocate(Type* pBlock)
//		{
//			assert(pBlock);
//
//			/* THIS IS EXTREMELY HARDCODED, BUT THIS COULD ONLY BE SOLVED VIA REFLECTION */
//
//			// BlockInformation* Block{ reinterpret_cast<BlockInformation*>(reinterpret_cast<char*>(&pBlock) - sizeof(size_t)) };
//			BlockInformation* Block{ reinterpret_cast<BlockInformation*>(reinterpret_cast<char*>(pBlock) - sizeof(size_t)) };
//
//			size_t bucket{ Block->BlockSize / sizeof(void*) - 1 };
//
//			if (bucket >= AmountOfLists)
//			{
//				bucket = AmountOfLists - 1;
//			}
//
//			FreeBlockLists[bucket].push(pBlock);
//		}
//
//		template<typename Type, typename ... Values>
//		static void construct(Type* p, Values&&... value)
//		{
//			new (p) Type(std::forward<Values>(value)...);
//		}
//
//		template<typename Type>
//		static void destroy(Type* p)
//		{
//			if constexpr (!std::is_trivially_destructible_v<Type>)
//			{
//				if (p)
//				{
//					p->~Type();
//				}
//			}
//		}
//
//	private:
//		static BlockInformation* GetFreeBlock(size_t bucket);
//
//		inline static std::vector<std::vector<void*>> BlockLists =
//		{
//			std::vector<void*>{}, /* 8 bytes */
//			std::vector<void*>{}, /* 16 bytes */
//			std::vector<void*>{}, /* 32 bytes */
//			std::vector<void*>{}, /* 64 bytes */
//			std::vector<void*>{}, /* 128 bytes */
//			std::vector<void*>{} /* > 128 bytes */
//		};
//
//		inline static std::vector<std::queue<void*>> FreeBlockLists =
//		{
//			std::queue<void*>{}, /* 8 bytes */
//			std::queue<void*>{}, /* 16 bytes */
//			std::queue<void*>{}, /* 32 bytes */
//			std::queue<void*>{}, /* 64 bytes */
//			std::queue<void*>{}, /* 128 bytes */
//			std::queue<void*>{} /* > 128 bytes */
//		};
//
//		inline static constexpr size_t AmountOfLists{ 6 };
//	};
//}