#pragma once

#include "../ComponentArray/ComponentArray.h"
#include "../SparseSet/SparseSet.h"

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
		View() = default;
		View(ViewContainerType&& components, const SparseSet<Entity>& entities)
			: m_Components{ std::move(components) }
			, Entities{ entities }
		{}

		View(const View&) noexcept = delete;
		View& operator=(const View&) noexcept = delete;

		void ForEach(const std::function<void(Ts&...)>& function) const
		{
			auto indexSequence{ std::make_index_sequence<sizeof ... (Ts)>{} };

			for (const Entity entity : Entities)
			{
				ForEachImpl(function, entity, indexSequence);
			}
		}

	private:
		template<size_t ... Is>
		void ForEachImpl(const std::function<void(Ts&...)>& function, const Entity ent, const std::index_sequence<Is...>&) const
		{
			if ((ent != InvalidEntityID) && (std::get<Is>(m_Components).HasEntity(ent) && ...))
				std::apply(function, std::tuple<Ts&...>(std::get<Is>(m_Components).GetComponent(ent)...));
		}

		ViewContainerType m_Components;
		const SparseSet<Entity>& Entities;
	};
}