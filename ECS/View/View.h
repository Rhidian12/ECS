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
		explicit View(ViewContainerType&& components, std::unordered_map<Entity, EntitySignature>& sigs)
			: Components{ std::move(components) }
			, EntitySignatures{ sigs }
			, Entities{}
		{
			SetEntities(std::make_index_sequence<sizeof ... (Ts)>{});
		}

		void ForEach(const std::function<void(Ts&...)>& function)
		{
			auto indexSequence{ std::make_index_sequence<sizeof ... (Ts)>{} };

			for (const std::vector<Entity>& list : *Entities)
			{
				for (const Entity ent : list)
				{
					ForEach(function, ent, indexSequence);
				}
			}
		}

	private:
		template<size_t ... Is>
		void ForEach(const std::function<void(Ts&...)>& function, const Entity ent, std::index_sequence<Is...>)
		{
			if ((EntitySignatures.at(ent).test(GenerateComponentID<Ts>()) && ...))
			{
				auto tuple{ std::tuple<Ts&...>(std::get<Is>(Components).GetComponent(ent)...) };
				std::apply(function, tuple);
			}
		}

		template<size_t ... Is>
		void SetEntities(std::index_sequence<Is...>)
		{
			((Entities = std::get<0>(Components).GetNrOfEntities() > std::get<Is>(Components).GetNrOfEntities() ?
				&std::get<0>(Components).GetEntities() : &std::get<Is>(Components).GetEntities()), ...);
		}

		ViewContainerType Components;
		std::unordered_map<Entity, EntitySignature>& EntitySignatures;
		std::vector<std::vector<Entity>>* Entities;
	};
}