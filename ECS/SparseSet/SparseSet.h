#pragma once

#include <vector> /* std::vector */
#include <assert.h> /* assert() */
#include <utility> /* std::pair */

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

	template<typename Type>
	class RandomConstIterator final
	{
	public:
		using difference_type = std::ptrdiff_t;

		RandomConstIterator(Type* pPointer)
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

			Sparse.reserve(std::numeric_limits<T>::max());
			Packed.reserve(std::numeric_limits<T>::max());
		}

		bool Add(const T& value)
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
				Sparse.resize(value + 1);
			}

			Packed[_Size] = value;
			Sparse[value] = _Size++;

			return true;
		}
		/* [TODO]: Make r-value overload */

		bool Contains(const T value) const { return (value < Sparse.size()) && (Packed[Sparse[value]] == value); }

		T Find(const T value) { assert(Sparse[value] < _Size); assert(Packed[Sparse[value]] == value); return Sparse[value]; }
		const T& Find(const T value) const { assert(Sparse[value] < _Size); assert(Packed[Sparse[value]] == value); return Sparse[value]; }

		size_t Size() const { return _Size; }
		void Clear() { Sparse.clear(); Packed.clear(); _Size = 0; }

		bool Remove(const T value)
		{
			if (Contains(value))
			{
				--_Size;

				Packed[Sparse[value]] = Packed[_Size];
				Sparse[Packed[_Size]] = Sparse[value];

				return true;
			}

			return false;
		}

		 RandomIterator<T> begin() noexcept { return RandomIterator(Packed.data()); }
		 RandomConstIterator<T> begin() const noexcept { return RandomConstIterator(Packed.data()); }
		 
		 RandomIterator<T> end() noexcept { return RandomIterator(Packed.data() + static_cast<T>(Packed.size())); }
		 RandomConstIterator<T> end() const noexcept { return RandomConstIterator(Packed.data() + static_cast<T>(Packed.size())); }
		 
		 /* [TODO]: Figure out why this causes errors */
		 //RandomConstIterator<SparseValue> cbegin() const noexcept { return RandomConstIterator(Packed.data()); }
		 //RandomConstIterator<SparseValue> cend() const noexcept { return RandomConstIterator<SparseValue>(Packed.data() + Packed.size()); }

	private:
		std::vector<T> Sparse;
		std::vector<T> Packed;
		T _Size;
	};
}