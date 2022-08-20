#pragma once

#include <stdexcept> /* std::bad_alloc */

namespace ECS
{
	class Allocator final
	{
	public:
		Allocator();
		explicit Allocator(const size_t totalSize);

		~Allocator();

		template<typename T>
		T* Allocate(const size_t nrOfElements)
		{
			const size_t totalSize{ sizeof(T) * nrOfElements };

			if (totalSize < Capacity - StackPointer)
			{
				const T* const pData{ static_cast<T*>(pBuffer + StackPointer) };
				StackPointer += totalSize;
				return pData;
			}
			else
			{
				/* Reallocate() */
			}

			throw std::bad_alloc{};
		}

		void Reallocate(const size_t newCapacity);

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
		}

		void* pBuffer;
		size_t StackPointer;
		size_t Capacity;
	};
}