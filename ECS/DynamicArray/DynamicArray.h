#pragma once
#include <assert.h>

#include "../PoolAllocator/PoolAllocator.h"

namespace ECS
{
	template<typename Type>
	class DynamicArray final
	{
		inline static constexpr size_t SizeOfType = sizeof(Type);
	public:
		~DynamicArray();

		/* If these aren't marked as noexcept, VS keeps complaining */
		DynamicArray(const DynamicArray& other) noexcept;
		DynamicArray(DynamicArray&& other) noexcept;
		DynamicArray& operator=(const DynamicArray& other) noexcept;
		DynamicArray& operator=(DynamicArray&& other) noexcept;

		void Add(const Type& val);
		void Add(Type&& val);

		template<typename ... Values>
		void Emplace(Values&&... val);

		void Pop();

		void Clear();

		size_t Size() const;

		size_t Capacity() const;

		void Reserve(size_t newCapacity);

		void Resize(size_t newSize);

		void ShrinkToFit();

		Type& Front();
		const Type& Front() const;

		Type& Back();
		const Type& Back() const;

		bool IsEmpty() const;

		Type& At(size_t index);
		const Type& At(size_t index) const;

		Type& operator[](size_t index);
		const Type& operator[](size_t index) const;

		Type* const Data() const;

	private:
		void ReleaseMemory(Type*& pOldHead);

		void DeleteData(Type* pHead, Type* const pTail);

		void Reallocate();
		void Reallocate(size_t newCapacity);

		template<typename ... Values>
		void ReallocateAndEmplace(Values&&... values);

		void ResizeToBigger(size_t newSize);
		void ResizeToSmaller(size_t newSize);

		Type* Head{ nullptr };
		Type* Tail{ nullptr };
		Type* CurrentElement{ nullptr };
	};

	template<typename Type>
	DynamicArray<Type>::~DynamicArray()
	{
		DeleteData(Head, Tail);
		ReleaseMemory(Head);
	}

	template<typename Type>
	DynamicArray<Type>::DynamicArray(const DynamicArray<Type>& other) noexcept
	{
		const size_t capacity{ other.Capacity() };

		Head = static_cast<Type*>(malloc(SizeOfType * capacity));
		Tail = Head + capacity;

		for (size_t index{}; index < other.Size(); ++index)
		{
			CurrentElement = Head + index;
			*CurrentElement = *(other.Head + index);
		}
	}

	template<typename Type>
	DynamicArray<Type>::DynamicArray(DynamicArray<Type>&& other) noexcept
		: Head{ std::move(other.Head) }
		, Tail{ std::move(other.Tail) }
		, CurrentElement{ std::move(other.CurrentElement) }
	{
		/* Don't release memory you're not reallocating fucking dumbass */
		other.Head = nullptr;
		other.Tail = nullptr;
		other.CurrentElement = nullptr;
	}

	template<typename Type>
	DynamicArray<Type>& DynamicArray<Type>::operator=(const DynamicArray<Type>& other) noexcept
	{
		const size_t capacity{ other.Capacity() };

		Head = static_cast<Type*>(malloc(SizeOfType * capacity));
		Tail = Head + capacity;

		for (size_t index{}; index < other.Size(); ++index)
		{
			CurrentElement = Head + index;
			*CurrentElement = *(other.Head + index);
		}

		return *this;
	}

	template<typename Type>
	DynamicArray<Type>& DynamicArray<Type>::operator=(DynamicArray<Type>&& other) noexcept
	{
		Head = std::move(other.Head);
		Tail = std::move(other.Tail);
		CurrentElement = std::move(other.CurrentElement);

		/* Don't release memory you're not reallocating fucking dumbass */

		other.Head = nullptr;
		other.Tail = nullptr;
		other.CurrentElement = nullptr;

		return *this;
	}

	template<typename Type>
	void DynamicArray<Type>::Add(const Type& val)
	{
		Emplace(val);
	}

	template<typename Type>
	void DynamicArray<Type>::Add(Type&& val)
	{
		Emplace(std::move(val));
	}

	template<typename Type>
	template<typename ...Values>
	void DynamicArray<Type>::Emplace(Values&&... val)
	{
		Type* const pNextBlock{ CurrentElement != nullptr ? CurrentElement + 1 : Head };

		if (!pNextBlock || pNextBlock >= Tail)
		{
			ReallocateAndEmplace(std::forward<Values>(val)...);
		}
		else
		{
			// *pNextBlock = Type(std::forward<Values>(val)...);

			// CurrentElement = pNextBlock;

			PoolAllocator::construct<Type>(pNextBlock, val...);
			CurrentElement = pNextBlock;
		}
	}

	template<typename Type>
	void DynamicArray<Type>::Pop()
	{
		if (Size() <= 0)
			return;

		DeleteData(CurrentElement, CurrentElement);

		Type* pPreviousBlock{ CurrentElement - 1 };

		if (pPreviousBlock < Head)
		{
			pPreviousBlock = nullptr;
		}

		CurrentElement = nullptr;
		CurrentElement = pPreviousBlock;
	}

	template<typename Type>
	void DynamicArray<Type>::Clear()
	{
		DeleteData(Head, Tail);

		CurrentElement = nullptr;
	}

	template<typename Type>
	size_t DynamicArray<Type>::Size() const
	{
		return CurrentElement != nullptr ? CurrentElement - Head + 1 : 0;
	}

	template<typename Type>
	size_t DynamicArray<Type>::Capacity() const
	{
		return Tail - Head;
	}

	template<typename Type>
	void DynamicArray<Type>::Reserve(size_t newCapacity)
	{
		if (newCapacity <= Capacity())
			return;

		Reallocate(newCapacity);
	}

	template<typename Type>
	void DynamicArray<Type>::Resize(size_t newSize)
	{
		if (newSize > Size())
		{
			ResizeToBigger(newSize);
		}
		else if (newSize < Size())
		{
			ResizeToSmaller(newSize);
		}
	}

	template<typename Type>
	void DynamicArray<Type>::ShrinkToFit()
	{
		if (Size() == Capacity())
			return;

		Reallocate(Size());
	}

	template<typename Type>
	Type& DynamicArray<Type>::Front()
	{
		assert(Head != nullptr);

		return *(Head);
	}

	template<typename Type>
	const Type& DynamicArray<Type>::Front() const
	{
		assert(Head != nullptr);

		return *(Head);
	}

	template<typename Type>
	Type& DynamicArray<Type>::Back()
	{
		assert(CurrentElement != nullptr);

		return *CurrentElement;
	}

	template<typename Type>
	const Type& DynamicArray<Type>::Back() const
	{
		assert(CurrentElement != nullptr);

		return *CurrentElement;
	}

	template<typename Type>
	bool DynamicArray<Type>::IsEmpty() const
	{
		return CurrentElement == nullptr;
	}

	template<typename Type>
	Type& DynamicArray<Type>::At(size_t index)
	{
		assert((Head + index) < Tail);

		return *(Head + index);
	}

	template<typename Type>
	const Type& DynamicArray<Type>::At(size_t index) const
	{
		assert((Head + index) < Tail);

		return *(Head + index);
	}

	template<typename Type>
	Type& DynamicArray<Type>::operator[](size_t index)
	{
		return *(Head + index);
	}

	template<typename Type>
	const Type& DynamicArray<Type>::operator[](size_t index) const
	{
		return *(Head + index);
	}

	template<typename Type>
	Type* const DynamicArray<Type>::Data() const
	{
		return Head;
	}

	template<typename Type>
	void DynamicArray<Type>::ReleaseMemory(Type*& pOldHead)
	{
		if (pOldHead)
		{
			PoolAllocator::deallocate(pOldHead);
		}
	}

	template<typename Type>
	void DynamicArray<Type>::DeleteData(Type* pHead, Type* const pTail)
	{
		if constexpr (!std::is_trivially_destructible_v<Type>) // if this is a struct / class with a custom destructor, call it
		{
			while (pHead <= pTail)
			{
				PoolAllocator::destroy(pHead);
				++pHead;
			}
		}
	}

	template<typename Type>
	void DynamicArray<Type>::Reallocate()
	{
		if (CurrentElement)
			assert(((CurrentElement + 1) >= Tail));

		const size_t oldCapacity{ size_t(Tail - Head) }; // also oldSize
		const size_t newCapacity{ oldCapacity != 0 ? oldCapacity * 2 : 1 };

		Type* pOldHead{ Head };
		Type* const pOldTail{ Tail };

		// Head = static_cast<Type*>(malloc(SizeOfType * newCapacity));
		Head = PoolAllocator::allocate(newCapacity);
		Tail = Head + newCapacity;

		for (size_t index{}; index < oldCapacity; ++index)
		{
			CurrentElement = Head + index; // adjust pointer
			*CurrentElement = std::move(*(pOldHead + index)); // move element from old memory over
		}

		DeleteData(pOldHead, pOldTail);
		ReleaseMemory(pOldHead);
	}

	template<typename Type>
	void DynamicArray<Type>::Reallocate(size_t newCapacity)
	{
		const size_t size{ Size() };

		Type* pOldHead{ Head };
		Type* const pOldTail{ Tail };

		// Head = static_cast<Type*>(malloc(SizeOfType * newCapacity));
		Head = PoolAllocator::allocate(newCapacity);
		Tail = Head + newCapacity;

		for (size_t index{}; index < size; ++index)
		{
			CurrentElement = Head + index; // adjust pointer
			*CurrentElement = std::move(*(pOldHead + index)); // move element from old memory over
		}

		DeleteData(pOldHead, pOldTail);
		ReleaseMemory(pOldHead);
	}

	template<typename Type>
	void DynamicArray<Type>::ResizeToBigger(size_t newSize)
	{
		static_assert(std::is_trivially_constructible_v<Type>, "Container::Resize() > Type is not default constructable!");

		const size_t sizeDifference{ newSize - Size() };

		for (size_t i{}; i < sizeDifference; ++i)
		{
			Emplace(Type{});
		}
	}

	template<typename Type>
	void DynamicArray<Type>::ResizeToSmaller(size_t newSize)
	{
		const size_t sizeDifference{ Size() - newSize };

		for (size_t i{}; i < sizeDifference; ++i)
		{
			Pop();
		}
	}

	template<typename Type>
	template<typename ...Values>
	void DynamicArray<Type>::ReallocateAndEmplace(Values && ...values)
	{
		Reallocate();

		if (!CurrentElement)
		{
			CurrentElement = Head;
		}
		else
		{
			++CurrentElement;
		}

		/* [TODO]: AN ALLOCATOR SHOULD DO THIS */
		// CurrentElement = new Type(std::forward<Values>(val)...);
		// *CurrentElement = Type(std::forward<Values>(values)...);

		PoolAllocator::construct<Type>(CurrentElement, val...);
	}
}