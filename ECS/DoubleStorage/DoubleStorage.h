#pragma once

#include <algorithm> /* std::find */
#include <vector> /* std::vector */

namespace ECS
{
	template<typename TKey, typename TValue>
	class DoubleStorage final
	{
	public:
		DoubleStorage() = default;

		TValue& Add(const TKey& key, TValue&& val)
		{
			Keys.emplace_back(key);
			Values.emplace_back(std::forward<TValue>(val));

			return Values.back();
		}

		void Remove(const TKey& key)
		{
			auto it = std::find(Keys.begin(), Keys.end(), key);
			auto index = it - Keys.begin();

			Keys.erase(it);
			Values.erase(Values.begin() + index);
		}

		TValue& GetValue(const TKey& key)
		{
			const auto index = std::find(Keys.begin(), Keys.end(), key) - Keys.begin();

			return Values[index];
		}
		const TValue& GetValue(const TKey& key) const
		{
			const auto index = std::find(Keys.cbegin(), Keys.cend(), key) - Keys.cbegin();

			return Values[index];
		}

		std::vector<TKey>& GetKeys() { return Keys; }
		const std::vector<TKey>& GetKeys() const { return Keys; }

		std::vector<TValue>& GetValues() { return Values; }
		const std::vector<TValue>& GetValues() const { return Values; }

	private:
		std::vector<TKey> Keys;
		std::vector<TValue> Values;
	};
}