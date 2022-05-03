#pragma once

#include <iostream>

namespace ECS
{
#ifdef NDEBUG
#define ASSERT(expression, message) ((void)0);
#else
#define ASSERT(expression, message) if (!expression) { std::cout << message << "\n"; std::abort(); }
#endif

	template<typename Type>
	class RandomIterator final
	{
	public:
		using difference_type = std::ptrdiff_t;

		RandomIterator(Type* const pPointer)
			: Pointer{ pPointer }
		{}

		/* Default rule of 5 is good enough */
		RandomIterator(const RandomIterator&) noexcept = default;
		RandomIterator(RandomIterator&&) noexcept = default;
		RandomIterator& operator=(const RandomIterator&) noexcept = default;
		RandomIterator& operator=(RandomIterator&&) noexcept = default;

		RandomIterator& operator+=(difference_type diff) { Pointer += diff; return *this; }
		RandomIterator& operator-=(difference_type diff) { Pointer -= diff; return *this; }

		Type& operator*() { return *Pointer; }
		Type* operator->() { return Pointer; }
		Type& operator[](difference_type diff) { return Pointer[diff]; }

		const Type& operator*() const { return *Pointer; }
		const Type* operator->() const { return Pointer; }
		const Type& operator[](difference_type diff) const { return Pointer[diff]; }

		RandomIterator& operator++() { ++Pointer; return *this; }
		RandomIterator& operator--() { --Pointer; return *this; }
		RandomIterator& operator++(int) { RandomIterator temp(*this); ++Pointer; return temp; }
		RandomIterator& operator--(int) { RandomIterator temp(*this); --Pointer; return temp; }

		difference_type operator-(const RandomIterator& other) const { return Pointer - other.Pointer; }

		RandomIterator operator+(difference_type diff) const { return RandomIterator(Pointer + diff); }
		RandomIterator operator-(difference_type diff) const { return RandomIterator(Pointer - diff); }

		friend RandomIterator operator+(difference_type diff, const RandomIterator& it) { return RandomIterator(diff + it.Pointer); }
		friend RandomIterator operator-(difference_type diff, const RandomIterator& it) { return RandomIterator(diff - it.Pointer); }

		bool operator==(const RandomIterator& it) const { return Pointer == it.Pointer; };
		bool operator!=(const RandomIterator& it) const { return Pointer != it.Pointer; };

		bool operator>(const RandomIterator& it) const { return Pointer > it.Pointer; };
		bool operator<(const RandomIterator& it) const { return Pointer < it.Pointer; };

		bool operator>=(const RandomIterator& it) const { return Pointer >= it.Pointer; };
		bool operator<=(const RandomIterator& it) const { return Pointer <= it.Pointer; };

	private:
		Type* Pointer{ nullptr };
	};

	template<typename Type>
	class RandomConstIterator final
	{
	public:
		using difference_type = std::ptrdiff_t;

		RandomConstIterator(Type* const pPointer)
			: Pointer{ pPointer }
		{}

		/* Default rule of 5 is good enough */
		RandomConstIterator(const RandomConstIterator&) noexcept = default;
		RandomConstIterator(RandomConstIterator&&) noexcept = default;
		RandomConstIterator& operator=(const RandomConstIterator&) noexcept = default;
		RandomConstIterator& operator=(RandomConstIterator&&) noexcept = default;

		RandomConstIterator& operator+=(difference_type diff) { Pointer += diff; return *this; }
		RandomConstIterator& operator-=(difference_type diff) { Pointer -= diff; return *this; }

		const Type& operator*() const { return *Pointer; }
		const Type* operator->() const { return Pointer; }
		const Type& operator[](difference_type diff) const { return Pointer[diff]; }

		RandomConstIterator& operator++() { ++Pointer; return *this; }
		RandomConstIterator& operator--() { --Pointer; return *this; }
		RandomConstIterator& operator++(int) { RandomConstIterator temp(*this); ++Pointer; return temp; }
		RandomConstIterator& operator--(int) { RandomConstIterator temp(*this); --Pointer; return temp; }

		difference_type operator-(const RandomConstIterator& other) const { return Pointer - other.Pointer; }

		RandomConstIterator operator+(difference_type diff) const { return RandomConstIterator(Pointer + diff); }
		RandomConstIterator operator-(difference_type diff) const { return RandomConstIterator(Pointer - diff); }

		friend RandomConstIterator operator+(difference_type diff, const RandomConstIterator& it) { return RandomConstIterator(diff + it.Pointer); }
		friend RandomConstIterator operator-(difference_type diff, const RandomConstIterator& it) { return RandomConstIterator(diff - it.Pointer); }

		bool operator==(const RandomConstIterator& it) const { return Pointer == it.Pointer; };
		bool operator!=(const RandomConstIterator& it) const { return Pointer != it.Pointer; };

		bool operator>(const RandomConstIterator& it) const { return Pointer > it.Pointer; };
		bool operator<(const RandomConstIterator& it) const { return Pointer < it.Pointer; };

		bool operator>=(const RandomConstIterator& it) const { return Pointer >= it.Pointer; };
		bool operator<=(const RandomConstIterator& it) const { return Pointer <= it.Pointer; };

	private:
		Type* Pointer{ nullptr };
	};

	/* [TODO]: Make Insert() and Emplace_At() */
	template<typename Type>
	class CustomContainer final
	{
		inline static constexpr size_t SizeOfType = sizeof(Type);

	public:
		CustomContainer() = default;
		~CustomContainer()
		{
			DeleteData(Head, Tail);
			ReleaseMemory(Head);
		}

		/* If these aren't marked as noexcept, VS keeps complaining */
		CustomContainer(const CustomContainer& other) noexcept
		{
			const size_t capacity{ other.Capacity() };

			Head = new Type[capacity]();
			// Head = PoolAllocator::allocate<Type>(capacity);
			Tail = Head + capacity;

			for (size_t index{}; index < other.Size(); ++index)
			{
				new (LastElement) Type(*(other.Head + index));
				LastElement = Head + index;
				// PoolAllocator::construct(LastElement, *(other.Head + index));
			}
		}
		CustomContainer(CustomContainer&& other) noexcept
			: Head{ std::move(other.Head) }
			, Tail{ std::move(other.Tail) }
			, LastElement{ std::move(other.LastElement) }
		{
			/* Don't release memory you're not reallocating fucking dumbass */
			other.Head = nullptr;
			other.Tail = nullptr;
			other.LastElement = nullptr;
		}

		CustomContainer<Type>& operator=(const CustomContainer& other) noexcept
		{
			const size_t capacity{ other.Capacity() };

			Head = new Type[capacity]();
			// Head = PoolAllocator::allocate<Type>(capacity);
			Tail = Head + capacity;

			for (size_t index{}; index < other.Size(); ++index)
			{
				LastElement = Head + index;
				new (LastElement) Type(*(other.Head + index));
				// PoolAllocator::construct(LastElement, *(other.Head + index));
			}

			return *this;
		}
		CustomContainer<Type>& operator=(CustomContainer&& other) noexcept
		{
			Head = std::move(other.Head);
			Tail = std::move(other.Tail);
			LastElement = std::move(other.LastElement);

			/* Don't release memory you're not reallocating fucking dumbass */

			other.Head = nullptr;
			other.Tail = nullptr;
			other.LastElement = nullptr;

			return *this;
		}

		void Add(const Type& val)
		{
			Emplace(val);
		}
		void Add(Type&& val)
		{
			Emplace(std::move(val));
		}

		template<typename ... Values>
		void Emplace(Values&&... val)
		{
			Type* const pNextBlock{ LastElement != nullptr ? LastElement + 1 : Head };

			if (!pNextBlock || pNextBlock >= Tail)
			{
				ReallocateAndEmplace(std::forward<Values>(val)...);
			}
			else
			{
				/* [TODO]: AN ALLOCATOR SHOULD DO THIS */
				// CurrentElement = new Type(std::forward<Values>(val)...);

				new (pNextBlock) Type(std::forward<Values>(val)...);
				// PoolAllocator::construct(pNextBlock, std::forward<Values>(val)...);

				LastElement = pNextBlock;
			}
		}

		void Pop()
		{
			ASSERT(LastElement, "No elements to be popped!");

			if constexpr (!std::is_trivially_destructible_v<Type>)
			{
				LastElement->~Type();
			}

			Type* pPreviousBlock{ LastElement - 1 > Head ? LastElement - 1 : nullptr };

			LastElement = nullptr;
			LastElement = pPreviousBlock;
		}

		void Clear()
		{
			DeleteData(Head, Tail);

			LastElement = nullptr;
		}

		size_t Size() const
		{
			return LastElement != nullptr ? LastElement - Head + 1 : 0;
		}

		size_t Capacity() const
		{
			return Tail - Head;
		}

		void Reserve(size_t newCapacity)
		{
			if (newCapacity <= Capacity())
				return;

			Reallocate(newCapacity);
		}

		void Resize(size_t newSize)
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

		void ShrinkToFit()
		{
			if (Size() == Capacity())
				return;

			Reallocate(Size());
		}

		Type& Front()
		{
			ASSERT((Head != nullptr), "Container::Front() > Out of range!");

			return *(Head);
		}
		const Type& Front() const
		{
			ASSERT((Head != nullptr), "Container::Front() > Out of range!");

			return *(Head);
		}

		Type& Back()
		{
			ASSERT((LastElement != nullptr), "Container::Back() > Out of range!");

			return *LastElement;
		}
		const Type& Back() const
		{
			ASSERT((LastElement != nullptr), "Container::Back() > Out of range!");

			return *LastElement;
		}

		bool IsEmpty() const
		{
			return LastElement == nullptr;
		}

		Type& At(size_t index)
		{
			ASSERT(((Head + index) < Tail), "Container::At() > Index was out of range!");

			return *(Head + index);
		}
		const Type& At(size_t index) const
		{
			ASSERT(((Head + index) < Tail), "Container::At() > Index was out of range!");

			return *(Head + index);
		}

		Type& operator[](size_t index)
		{
			return *(Head + index);
		}
		const Type& operator[](size_t index) const
		{
			return *(Head + index);
		}

		Type* const Data()
		{
			return Head;
		}
		const Type* const Data() const
		{
			return Head;
		}

		RandomIterator<Type> begin() noexcept { return RandomIterator(Head); }
		RandomConstIterator<Type> begin() const noexcept { return RandomConstIterator(Head); }

		RandomIterator<Type> end() noexcept { return RandomIterator(LastElement + 1); }
		RandomConstIterator<Type> end() const noexcept { return RandomConstIterator(LastElement + 1); }

		RandomConstIterator<Type> cbegin() const noexcept { return RandomConstIterator(Head); }
		RandomConstIterator<Type> cend() const noexcept { return RandomConstIterator(LastElement + 1); }

	private:
		void ReleaseMemory(Type*& pOldHead)
		{
			if (pOldHead)
			{
				delete[] pOldHead;
				pOldHead = nullptr;
			}

			//if (pOldHead)
			//{
			//	PoolAllocator::deallocate(pOldHead);
			//}
		}

		void DeleteData(Type* pHead, Type* const pTail)
		{
			if constexpr (!std::is_trivially_destructible_v<Type>) // if this is a struct / class with a custom destructor, call it
			{
				while (pHead <= pTail)
				{
					// PoolAllocator::destroy(pHead);
					pHead->~Type();
					++pHead;
				}
			}
		}

		void Reallocate(size_t newCapacity)
		{
			const size_t size{ Size() };

			Type* pOldHead{ Head };
			Type* const pOldTail{ Tail };

			Head = static_cast<Type*>(malloc(SizeOfType * newCapacity));
			// Head = PoolAllocator::allocate<Type>(newCapacity);
			Tail = Head + newCapacity;

			for (size_t index{}; index < size; ++index)
			{
				LastElement = Head + index; // adjust pointer
				*LastElement = std::move(*(pOldHead + index)); // move element from old memory over
			}

			DeleteData(pOldHead, pOldTail);
			ReleaseMemory(pOldHead);
		}

		template<typename ... Values>
		void ReallocateAndEmplace(Values && ...values)
		{
			Reallocate(Size() + Size() / 2 + 1);

			if (!LastElement)
			{
				LastElement = Head;
			}
			else
			{
				++LastElement;
			}

			/* [TODO]: AN ALLOCATOR SHOULD DO THIS */
			// CurrentElement = new Type(std::forward<Values>(val)...);

			new (LastElement) Type(std::forward<Values>(values)...);
			// PoolAllocator::construct(LastElement, std::forward<Values>(values)...);
		}

		void ResizeToBigger(size_t newSize)
		{
			static_assert(std::is_default_constructible_v<Type>, "Container::Resize() > Type is not default constructable!");

			const size_t sizeDifference{ newSize - Size() };

			for (size_t i{}; i < sizeDifference; ++i)
			{
				Emplace(Type{});
			}
		}
		void ResizeToSmaller(size_t newSize)
		{
			const size_t sizeDifference{ Size() - newSize };

			for (size_t i{}; i < sizeDifference; ++i)
			{
				Pop();
			}
		}

		Type* Head{ nullptr };
		Type* Tail{ nullptr };
		Type* LastElement{ nullptr };
	};
}