#pragma once

#include <assert.h> 
#include <vector> 

namespace ECS
{
	template<typename T>
	class DenseSet final
	{
	public:
		DenseSet()
			: Sparse{}
			, Packed{}
			, _Size{}
		{
			static_assert(std::is_integral_v<T>, "SparseSet only supports integer types");
		}

		/* Default Rule of 5 is sufficient */
		template<typename U, typename V>
		bool Add(U&& valOne, V&& valTwo)
		{
			if (Contains(valOne))
			{
				return false;
			}

			if (Packed.size() <= _Size)
			{
				Packed.resize(_Size + 1);
			}
			if (Sparse.size() <= valOne)
			{
				Sparse.resize(valOne + 1, InvalidEntityID);
			}

			Packed[_Size] = std::make_pair(valOne, valTwo);
			Sparse[valOne] = _Size++;

			return true;
		}

		bool Contains(const T value) const
		{
			return (value < Sparse.size()) && (Sparse[value] != InvalidEntityID);
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

		__forceinline T GetFirst(const T val) const { assert(Contains(val)); return Packed[Sparse[val]].first; }
		__forceinline T GetSecond(const T val) const { assert(Contains(val)); return Packed[Sparse[val]].second; }

	private:
		std::vector<T> Sparse;
		std::vector<std::pair<T, T>> Packed;
		T _Size;
	};
}