#pragma once

#include "../Memory/Memory.h"
#include "../DoubleStorage/DoubleStorage.h"

#include <vector> /* std::vector */
#include <assert.h> /* assert() */
#include <utility> /* std::move(), ... */
#include <functional> /* std::function, std::reference_wrapper */
#include <array> /* std::array */

namespace ECS
{
	template<typename ... TComponents>
	class View final
	{
		using ViewContainerType = std::tuple<DoubleStorage<Entity, TComponents>&...>;

	public:
		explicit View(ViewContainerType&& components, std::unordered_map<Entity, EntitySignature>& sigs)
			: Components{ std::move(components) }
			, EntitySignatures{ sigs }
			, Entities{ std::get<0>(Components).GetKeys() }
		{
			SetEntities(std::make_index_sequence<sizeof ... (TComponents)>{});
		}

		void ForEach(const std::function<void(TComponents&...)>& function)
		{
			auto indexSequence{ std::make_index_sequence<sizeof ... (TComponents)>{} };

			for (Entity entity : Entities)
			{
				ForEach(function, std::move(entity), indexSequence);
			}
		}

	private:
		template<size_t ... Indices>
		void ForEach(const std::function<void(TComponents&...)>& function, Entity&& ent, std::index_sequence<Indices...>)
		{
			if ((EntitySignatures.at(ent).test(GenerateComponentID<TComponents>()) && ...))
			{
				auto tuple{ std::tuple<TComponents&...>(std::get<Indices>(Components).GetValue(ent)...)};
				std::apply(function, tuple);
			}
		}

		template<size_t ... Is>
		void SetEntities(std::index_sequence<Is...>)
		{
			((Entities = std::get<Is>(Components).GetKeys().size() < Entities.size() ? std::get<Is>(Components).GetKeys() : Entities), ...);
		}

		ViewContainerType Components;
		std::unordered_map<Entity, EntitySignature>& EntitySignatures;
		std::vector<Entity>& Entities;
	};
}