#pragma once

#include "../DoubleStorage/DoubleStorage.h"
#include "../ComponentArray/ComponentArray.h"

#include <vector> /* std::vector */
#include <assert.h> /* assert() */
#include <utility> /* std::move(), ... */
#include <functional> /* std::function, std::reference_wrapper */
#include <array> /* std::array */

namespace ECS
{
	template<typename ... Ts>
	class View final
	{
		using ViewContainerType = std::tuple<ComponentArray<Ts>&...>;

	public:
		explicit View(ViewContainerType&& components, std::unordered_map<Entity, EntitySignature>& sigs,
			const std::vector<std::vector<Entity>>& entities)
			: Components{ std::move(components) }
			, EntitySignatures{ sigs }
			, Entities{ entities }
		{}

		void ForEach(const std::function<void(Ts&...)>& function)
		{
			auto indexSequence{ std::make_index_sequence<sizeof ... (Ts)>{} };

			for (const std::vector<Entity>& list : Entities)
			{
				for (const Entity ent : list)
				{
					ForEach(function, ent, indexSequence);
				}
			}
		}

	private:
		template<size_t ... Indices>
		void ForEach(const std::function<void(Ts&...)>& function, const Entity ent, std::index_sequence<Indices...>)
		{
			if ((EntitySignatures.at(ent).test(GenerateComponentID<Ts>()) && ...))
			{
				auto tuple{ std::tuple<Ts&...>(std::get<Indices>(Components).GetComponent(ent)...) };
				std::apply(function, tuple);
			}
		}

		ViewContainerType Components;
		std::unordered_map<Entity, EntitySignature>& EntitySignatures;
		const std::vector<std::vector<Entity>>& Entities;
	};
}