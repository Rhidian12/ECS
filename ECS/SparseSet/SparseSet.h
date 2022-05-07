#pragma once

#include <vector> /* std::vector */
#include <assert.h> /* assert() */

namespace ECS
{
	template<typename SparseValue>
	class SparseSet final
	{
	public:
		void Add(const SparseValue& value)
		{
			PackedSet.push_back(value);

			if (value >= SparseSet.size())
			{
				SparseSet.resize(value + 1);
			}

			SparseSet[value] = Size++;
		}
		void Add(SparseValue&& value)
		{
			PackedSet.push_back(value);

			if (value >= SparseSet.size())
			{
				SparseSet.resize(value + 1);
			}

			SparseSet[value] = Size++;
		}

		SparseValue& Find(const SparseValue& value)
		{
			assert(value < PackedSet.size());

			// if ()
		}

	private:
		std::vector<SparseValue> SparseSet;
		std::vector<SparseValue> PackedSet;
		size_t Size;
	};
}