#pragma once

#include "../DoubleStorage/DoubleStorage.h"
#include "../ComponentArray/ComponentArray.h"

#include <bitset>
#include <functional> /* std::function, std::reference_wrapper */
#include <utility> /* std::move(), ... */
#include <vector> /* std::vector */

namespace ECS
{
	template<typename ... Ts>
	class View final
	{
		using ViewContainerType = std::tuple<ComponentArray<Ts>&...>;

	public:
		explicit View(ViewContainerType&& components, const std::vector<std::pair<Entity, EntitySignature>>& sigs)
			: Components{ std::move(components) }
			, EntitySignatures{ sigs }
		{}

		void ForEach(const std::function<void(Ts&...)>& function)
		{
			auto indexSequence{ std::make_index_sequence<sizeof ... (Ts)>{} };

			for (const auto& [entity, sig] : EntitySignatures)
				ForEach(function, entity, sig, indexSequence);
		}

	private:
		template<size_t ... Is>
		void ForEach(const std::function<void(Ts&...)>& function, const Entity ent, const EntitySignature& sig, const std::index_sequence<Is...>&)
		{
			if ((ent != InvalidEntityID) && (sig.test(GenerateComponentID<Ts>()) && ...))
				std::apply(function, std::tuple<Ts&...>(std::get<Is>(Components).GetComponent(ent)...));
		}

		ViewContainerType Components;
		const std::vector<std::pair<Entity, EntitySignature>>& EntitySignatures;
	};
}