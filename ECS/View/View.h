#pragma once

#include <vector> /* std::vector */
#include <assert.h> /* assert() */
#include <utility> /* std::move(), ... */
#include <functional> /* std::function, std::reference_wrapper */
#include <array> /* std::array */

#include "../Memory/Memory.h"

namespace ECS
{
	template<typename ... TComponents>
	class View final
	{
		using ViewContainerType = std::tuple<std::vector<std::reference_wrapper<TComponents>, STLAllocator<std::reference_wrapper<TComponents>, StackAllocator>>...>;

	public:
		explicit View(ViewContainerType&& components)
			: Components{ std::move(components) }
			, NrOfComponents{ std::get<0>(Components).size() }
		{}

		void ForEach(const std::function<void(TComponents&...)>& function)
		{
			auto indexSequence{ std::make_index_sequence<sizeof ... (TComponents)>{} };

			for (size_t i{}; i < NrOfComponents; ++i)
			{
				ForEach(function, i, indexSequence);
			}
		}

	private:
		template<size_t ... Indices>
		void ForEach(const std::function<void(TComponents&...)>& function, size_t index, std::index_sequence<Indices...>)
		{
			auto tuple{ std::tuple<TComponents&...>(std::get<Indices>(Components)[index].get()...)};
			std::apply(function, tuple);
		}

		ViewContainerType Components;
		size_t NrOfComponents;
	};
}