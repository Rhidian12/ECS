#pragma once

#include "../Benchmark/BenchmarkUtils.h"

#include <vector> /* std::vector */
#include <assert.h> /* assert() */

namespace ECS
{
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

	template<typename T>
	class RandomConstIterator final
	{
	public:
		using difference_type = std::ptrdiff_t;

		RandomConstIterator(T* pPointer)
			: Pointer{ pPointer }
		{}

		/* Default rule of 5 is good enough */
		RandomConstIterator(const RandomConstIterator&) noexcept = default;
		RandomConstIterator(RandomConstIterator&&) noexcept = default;
		RandomConstIterator& operator=(const RandomConstIterator&) noexcept = default;
		RandomConstIterator& operator=(RandomConstIterator&&) noexcept = default;

		RandomConstIterator& operator+=(difference_type diff) { Pointer += diff; return *this; }
		RandomConstIterator& operator-=(difference_type diff) { Pointer -= diff; return *this; }

		const T& operator*() const { return *Pointer; }
		const T* operator->() const { return Pointer; }
		const T& operator[](difference_type diff) const { return Pointer[diff]; }

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
		T* Pointer{ nullptr };
	};

	template<typename T>
	class SparseSet final
	{
	public:
		SparseSet()
			: Sparse{}
			, Packed{}
			, _Size{}
		{
			static_assert(std::is_integral_v<T>, "SparseSet only supports integer types");
		}

		/* Default Rule of 5 is sufficient */

		bool Add(const T& value)
		{
			return AddImpl(value);
		}
		bool Add(T&& value)
		{
			return AddImpl(std::move(value));
		}

		bool Contains(const T value) const
		{
			return (value < Sparse.size()) && (Sparse[value] != InvalidEntityID);
		}
		T GetIndex(const T value) const { assert(Contains(value)); return Sparse[value]; }

		void Swap(const T a, const T b)
		{
			assert(Contains(a) && Contains(b));

			std::swap(Packed[Sparse[a]], Packed[Sparse[b]]);
		}

		size_t Size() const { return _Size; }
		void Clear() { Sparse.clear(); Packed.clear(); _Size = 0; }

		bool Remove(const T value)
		{
			if (Contains(value))
			{
				--_Size;

				Sparse[value] = InvalidEntityID;

				return true;
			}

			return false;
		}

		void Reserve(const size_t capacity) { Sparse.reserve(capacity); Packed.reserve(capacity); }

		T& operator[](const size_t index)
		{ 
			return Packed[Sparse[index]];
		}
		const T operator[](const size_t index) const
		{ 
			return Packed[Sparse[index]]; 
		}

		const std::vector<T>& GetSparse() const
		{
			return Sparse;
		}

		RandomIterator<T> begin() { return RandomIterator(Packed.data()); }
		RandomIterator<const T> begin() const { return RandomIterator<const T>(Packed.data()); }

		RandomIterator<T> end() { return RandomIterator{ Packed.data() + Packed.size() }; }
		RandomIterator<const T> end() const { return RandomIterator<const T>{ Packed.data() + Packed.size() }; }

		RandomConstIterator<T> cbegin() const { return RandomConstIterator(Packed.data()); }
		RandomConstIterator<T> cend() const { return RandomConstIterator(Packed.data() + Packed.size()); }

	private:
		template<typename U>
		bool AddImpl(U&& value)
		{
			if (Contains(value))
			{
				return false;
			}

			if (Packed.size() <= _Size)
			{
				Packed.resize(_Size + 1);
			}
			if (Sparse.size() <= value)
			{
				Sparse.resize(value + 1, InvalidEntityID);
			}

			Packed[_Size] = value;
			Sparse[value] = _Size++;

			return true;
		}

		std::vector<T> Sparse;
		std::vector<T> Packed;
		T _Size;
	};
}