#pragma once

#include <assert.h> /* assert() */
#include <cstdlib> /* malloc() */
#include <type_traits> /* std::type_traits */

/* Based on the following article: */
/* https://arjunsreedharan.org/ */

namespace ECS
{
	inline static constexpr size_t AlignSize{ 16 };

	struct alignas(AlignSize) BlockInformation final
	{
		size_t BlockSize;
		bool IsFree;

		BlockInformation* pNext;
		void* Data;
	};

	template<typename Type>
	class MemoryAllocator final
	{
	public:
		MemoryAllocator() = default;

		MemoryAllocator(const MemoryAllocator&) = delete;
		MemoryAllocator(MemoryAllocator&&) = delete;
		MemoryAllocator& operator=(const MemoryAllocator&) = delete;
		MemoryAllocator& operator=(MemoryAllocator&&) = delete;

		~MemoryAllocator();

		Type* allocate(size_t nrOfElementsToAllocate);

		/* unreferenced size_t for STL */
		void deallocate(void* pBlock, size_t) noexcept;

		void construct(Type* p, const Type& value);

		void destroy(Type* p);

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

			free(pTemp->Data);
			free(pTemp);

			pTemp = pNext;
		}
	}

	template<typename Type>
	Type* MemoryAllocator<Type>::allocate(size_t nrOfElementsToAllocate)
	{
		assert(nrOfElementsToAllocate != 0);

		BlockInformation* pBlockInfo{ GetFreeBlock(nrOfElementsToAllocate) };

		if (pBlockInfo)
		{
			pBlockInfo->IsFree = false;
			return reinterpret_cast<Type*>(pBlockInfo->Data);
		}

		const size_t totalSize{ sizeof(BlockInformation) + nrOfElementsToAllocate * sizeof(Type) };
		void* const pBlock{ malloc(totalSize) };

		assert(pBlock);

		pBlockInfo = static_cast<BlockInformation*>(pBlock);
		pBlockInfo->BlockSize = nrOfElementsToAllocate * sizeof(Type);
		pBlockInfo->IsFree = false;
		pBlockInfo->pNext = nullptr;
		pBlockInfo->Data = malloc(nrOfElementsToAllocate * sizeof(Type));

		if (!Head)
		{
			Head = pBlockInfo;
		}

		if (Tail)
		{
			Tail->pNext = pBlockInfo;
		}

		Tail = pBlockInfo;

		return reinterpret_cast<Type*>(pBlockInfo->Data);
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
	void MemoryAllocator<Type>::construct(Type* p, const Type& value)
	{
		::new (static_cast<void*>(p)) Type(value);
	}

	template<typename Type>
	void MemoryAllocator<Type>::destroy(Type* p)
	{
		p->~Type();
	}

	template<typename Type>
	BlockInformation* MemoryAllocator<Type>::GetFreeBlock(size_t size) const
	{
		BlockInformation* pCurrent{ Head };

		while (pCurrent)
		{
			if (pCurrent->IsFree && pCurrent->BlockSize >= size * sizeof(Type))
			{
				return pCurrent;
			}

			pCurrent = pCurrent->pNext;
		}

		return nullptr;
	}
}