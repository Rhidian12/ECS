#pragma once

#include <assert.h> /* assert() */
#include <cstdlib> /* malloc() */
#include <type_traits> /* std::type_traits */

/* Based on the following article: */
/* https://arjunsreedharan.org/ */

namespace ECS
{
	struct alignas(16) BlockInformation final
	{
		size_t BlockSize;
		bool IsFree;
		BlockInformation* pNext;
	};

	template<typename Type>
	class MemoryAllocator final
	{
	public:
		using value_type = Type;

		MemoryAllocator() = default;
		template<typename OtherType>
		constexpr MemoryAllocator(const MemoryAllocator<OtherType>&) noexcept {}

		~MemoryAllocator();

		Type* allocate(size_t size);

		/* unreferenced size_t for STL */
		void deallocate(void* pBlock, size_t) noexcept;

	private:
		BlockInformation* GetFreeBlock(size_t size) const;

		BlockInformation* Head{};
		BlockInformation* Tail{};
	};

	template<typename Type, typename OtherType>
	bool operator==(const Type&, const OtherType&) { return std::is_same_v<Type, OtherType>; }

	template<typename Type, typename OtherType>
	bool operator!=(const Type& a, const OtherType& b) { return !(a == b); }

	template<typename Type>
	MemoryAllocator<Type>::~MemoryAllocator()
	{
		BlockInformation* pTemp{ Head };

		while (pTemp)
		{
			BlockInformation* pNext{ pTemp->pNext };

			//free(pTemp);

			pTemp = pNext;
		}
	}

	template<typename Type>
	Type* MemoryAllocator<Type>::allocate(size_t size)
	{
		assert(size != 0);

		BlockInformation* pBlockInfo{ GetFreeBlock(size) };

		if (pBlockInfo)
		{
			pBlockInfo->IsFree = false;
			return reinterpret_cast<Type*>(pBlockInfo);
		}

		const size_t totalSize{ sizeof(BlockInformation) + size * sizeof(Type) };
		void* const pBlock{ malloc(totalSize) };

		assert(pBlock);

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

		return reinterpret_cast<Type*>(pBlockInfo);
	}
	
	template<typename Type>
	void MemoryAllocator<Type>::deallocate(void* pBlock, size_t) noexcept
	{
		assert(pBlock);

		BlockInformation* pBlockInfo{};
		BlockInformation* pTemp{};

		pBlockInfo = static_cast<BlockInformation*>(pBlock);

		pBlockInfo->IsFree = true;
	}

	template<typename Type>
	BlockInformation* MemoryAllocator<Type>::GetFreeBlock(size_t size) const
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