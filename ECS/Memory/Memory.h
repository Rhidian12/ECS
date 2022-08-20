#pragma once

#include <stdexcept> /* std::bad_alloc */

namespace ECS
{
	class StackAllocator final
	{
	public:
		StackAllocator();
		explicit StackAllocator(const size_t totalSize);

		StackAllocator(const StackAllocator&) noexcept = delete;
		StackAllocator(StackAllocator&& other) noexcept;
		StackAllocator& operator=(const StackAllocator&) noexcept = delete;
		StackAllocator& operator=(StackAllocator&& other) noexcept;

		~StackAllocator();

		template<typename T>
		T* Allocate(const size_t nrOfElements)
		{
			const size_t totalSize{ sizeof(T) * nrOfElements };

			if (totalSize < Capacity - StackPointer)
			{
				T* const pData{ reinterpret_cast<T*>(reinterpret_cast<size_t>(pBuffer) + StackPointer) };
				StackPointer += totalSize;
				return pData;
			}

			throw std::bad_alloc{};
		}

		template<typename T>
		void Deallocate(T* p, const size_t n)
		{
			assert(IsPointerInBuffer(p));

			StackPointer -= n * sizeof(T);
		}

		void Reallocate(const size_t newCapacity);

		void* Buffer() const { return pBuffer; }

		size_t GetCapacity() const { return Capacity; }

	private:
		size_t RoundToNextPowerOfTwo(size_t v) const
		{
			v--;
			v |= v >> 1u;
			v |= v >> 2u;
			v |= v >> 4u;
			v |= v >> 8u;
			v |= v >> 16u;
			v |= v >> 32u;
			v++;

			return v;
		}
		template<typename T>
		bool IsPointerInBuffer(const T* p) const
		{
			return std::greater_equal<const T*>{}(p, reinterpret_cast<const T*>(pBuffer)) && std::less<const T*>{}(p, reinterpret_cast<const T*>(reinterpret_cast<size_t>(pBuffer) + Capacity));
		}

		void* pBuffer;
		size_t StackPointer;
		size_t Capacity;
	};

	template<typename T, typename Allocator>
	class STLAllocator final
	{
	public:
		using value_type = T;

		STLAllocator() = delete;
		STLAllocator(Allocator& alloc)
			: _Allocator{ alloc }
		{}
		template<typename U>
		STLAllocator(const STLAllocator<U, Allocator>& other) noexcept
			: _Allocator(other._Allocator)
		{}

		T* allocate(const size_t nrOfElements)
		{
			return _Allocator.Allocate<T>(nrOfElements);
		}

		void deallocate(T* p, size_t n)
		{
			_Allocator.Deallocate<T>(p, n);
		}

		T* buffer() const
		{
			return reinterpret_cast<T*>(_Allocator.Buffer());
		}

		bool operator==(const STLAllocator<T, Allocator>& rhs) const noexcept
		{
			return buffer() == rhs.buffer();
		}

		bool operator!=(const STLAllocator<T, Allocator>& rhs) const noexcept
		{
			return buffer() != rhs.buffer();
		}

		Allocator& _Allocator;
	};
}