#pragma once
#include "../ECSConstants.h"

#include "../ComponentManager/ComponentManager.h"
#include "../EntityManager/EntityManager.h"
#include "../SparseSet/SparseSet.h"

#include <vector> /* std::vector */
#include <assert.h> /* assert() */
#include <utility> /* std::move(), ... */
#include <functional> /* std::function, std::reference_wrapper */

namespace ECS
{
	template<typename ... TComponents>
	class View final
	{
		using ViewContainerType = std::tuple<std::vector<TComponents>&...>;

	public:
		View(ViewContainerType&& components)
			: Components{ std::move(components) }
			, NrOfComponents{ std::get<0>(Components).size() }
		{}

		void ForEach(const std::function<void(TComponents&...)>& function) const
		{
			auto indexSequence{ std::make_index_sequence<sizeof ... (TComponents)>{} };

			for (size_t i{}; i < NrOfComponents; ++i)
			{
				ForEach(function, i, indexSequence);
			}
		}

	private:
		template<size_t ... Indices>
		void ForEach(const std::function<void(TComponents&...)>& function, size_t index, std::index_sequence<Indices...>) const
		{
			std::apply(function, std::tuple<TComponents&...>(std::get<Indices>(Components)[index]...));
		}

		ViewContainerType Components;
		size_t NrOfComponents;
	};

	class System final
	{
	public:
		System() = default;

		System(const System&) noexcept = delete;
		System(System&&) noexcept = delete;
		System& operator=(const System&) noexcept = delete;
		System& operator=(System&&) noexcept = delete;

		Entity CreateEntity();

		void ClearEntities();

		template<typename TComponent>
		void AddComponent(Entity entity) const
		{
			assert(entity != InvalidEntityID);
			assert(Entities.Contains(entity));

			ComponentManager::GetInstance()->AddComponent<TComponent>(entity);
			EntityManager::GetInstance()->SetEntitySignature(entity, TComponent::GetComponentID());
		}

		template<typename ... TComponents>
		View<TComponents...> CreateView() const
		{
			return View<TComponents...>(std::tuple<std::vector<TComponents>&...>(ComponentManager::GetInstance()->GetComponents<TComponents>()...));
		}

		template<typename TComponent>
		TComponent& GetComponent(Entity id) { assert(id != InvalidEntityID); assert(Entities.Contains(id)); return ComponentManager::GetInstance()->GetComponent<TComponent>(id); }
		template<typename TComponent>
		const TComponent& GetComponent(Entity id) const { assert(id != InvalidEntityID); assert(Entities.Contains(id)); return ComponentManager::GetInstance()->GetComponent<TComponent>(id); }
	
	private:
		template<typename TComponent>
		std::vector<TComponent> FilterEntities(const std::vector<TComponent>& v) const
		{
			constexpr ComponentType componentID(TComponent::GetComponentID());
			std::vector<TComponent> filtered(v.size());
			const EntityManager* const pEntityManager(EntityManager::GetInstance());
			Entity counter{};

			for (const Entity& entity : Entities)
			{
				if (pEntityManager->GetEntitySignature(entity)[componentID])
				{
					filtered[counter] = v[counter++];
				}
			}

			return filtered;
		}

		SparseSet<Entity> Entities{};
	};
}