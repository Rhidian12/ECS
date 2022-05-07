#pragma once

#include <vector> /* std::vector */
#include <assert.h> /* assert() */
#include <utility> /* std::pair */

namespace ECS
{
	template<typename SparseValue>
	class SparseSet final
	{
	public:
		void Add(const SparseValue& value)
		{
			const SparseValue& addedValue(PackedSet.emplace_back(value));

			if (addedValue >= SparseSet.size())
			{
				SparseSet.resize(addedValue + 1);
			}

			SparseSet[addedValue] = Size++;
		}
		void Add(SparseValue&& value)
		{
			const SparseValue& addedValue(PackedSet.emplace_back(std::make_pair(std::move(value), true)));

			if (addedValue >= SparseSet.size())
			{
				SparseSet.resize(addedValue + 1);
			}

			SparseSet[addedValue] = std::make_pair(Size++, true);
		}

		bool Contains(const SparseValue& value) const { assert(value < this->SparseSet.size()); return SparseSet[value].second; }

		SparseValue& Find(const SparseValue& value) { assert(value < this->SparseSet.size()); assert(this->SparseSet[value].second); return PackedSet[SparseSet[value].first].first; }
		const SparseValue& Find(const SparseValue& value) const { assert(value < this->SparseSet.size()); assert(this->SparseSet[value].second); return PackedSet[SparseSet[value].first].first; }

		SparseValue& Front() { assert(PackedSet.size() > 0); return PackedSet[0].first; }
		const SparseValue& Front() const { assert(PackedSet.size() > 0); return PackedSet[0].first; }

		SparseValue& Back() { assert(PackedSet.size() > 0); return PackedSet[Size].first; }
		const SparseValue& Back() const { assert(PackedSet.size() > 0); return PackedSet[Size].first; }

		void Clear() { SparseSet.clear(); PackedSet.clear(); }

	private:
		std::vector<std::pair<SparseValue, bool>> SparseSet;
		std::vector<std::pair<SparseValue, bool>> PackedSet;
		size_t Size;
	};
}