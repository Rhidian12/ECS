#pragma once

#include <vector> /* std::vector */
#include <assert.h> /* assert() */
#include <utility> /* std::move(), ... */
#include <functional> /* std::function, std::reference_wrapper */

namespace ECS
{
	template<typename ... Ts>
	class View final
	{
		using ViewContainerType = std::tuple<std::vector<Ts>&...>;

	public:
		explicit View(ViewContainerType&& components)
			: Components{ std::move(components) }
			, NrOfComponents{ std::get<0>(Components).size() }
		{}

		void ForEach(const std::function<void(Ts&...)>& function)
		{
			auto indexSequence{ std::make_index_sequence<sizeof ... (Ts)>{} };

			for (size_t i{}; i < NrOfComponents; ++i)
			{
				ForEach(function, i, indexSequence);
			}
		}

	private:
		template<size_t ... Indices>
		void ForEach(const std::function<void(Ts&...)>& function, size_t index, std::index_sequence<Indices...>)
		{
			auto tuple{ std::tuple<Ts&...>(std::get<Indices>(Components)[index]...)};
			std::apply(function, tuple);
		}

		ViewContainerType Components;
		size_t NrOfComponents;
	};
}