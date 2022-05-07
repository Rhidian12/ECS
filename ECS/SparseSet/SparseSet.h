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
			const SparseValue& addedValue(PackedSet.emplace_back(std::make_pair(value, true)).first);

			if (addedValue >= this->SparseSet.size())
			{
				this->SparseSet.resize(addedValue + 1);
			}

			this->SparseSet[addedValue] = Size++;
		}
		void Add(SparseValue&& value)
		{
			const SparseValue& addedValue(PackedSet.emplace_back(std::make_pair(std::move(value), true)).first);

			if (addedValue >= this->SparseSet.size())
			{
				this->SparseSet.resize(addedValue + 1);
			}

			this->SparseSet[addedValue] = std::make_pair(Size++, true);
		}

		bool Contains(const SparseValue& value) const { assert(value < this->SparseSet.size()); return this->SparseSet[value].second; }

		SparseValue& Find(const SparseValue& value) { assert(value < this->SparseSet.size()); assert(this->SparseSet[value].second); return PackedSet[SparseSet[value].first].first; }
		const SparseValue& Find(const SparseValue& value) const { assert(value < this->SparseSet.size()); assert(this->SparseSet[value].second); return PackedSet[SparseSet[value].first].first; }

		SparseValue& Front() { assert(PackedSet.size() > 0); return PackedSet[0].first; }
		const SparseValue& Front() const { assert(PackedSet.size() > 0); return PackedSet[0].first; }

		SparseValue& Back() { assert(PackedSet.size() > 0); return PackedSet[Size].first; }
		const SparseValue& Back() const { assert(PackedSet.size() > 0); return PackedSet[Size].first; }

		void Clear() { this->SparseSet.clear(); PackedSet.clear(); }

	private:
		std::vector<std::pair<SparseValue, bool>> SparseSet;
		std::vector<std::pair<SparseValue, bool>> PackedSet;
		SparseValue Size;
	};
}