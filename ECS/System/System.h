#if 0
#pragma once
#include "../ECSConstants.h"

#include "../ComponentManager/ComponentManager.h"
#include "../EntityManager/EntityManager.h"

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
		using ViewContainerType = std::tuple<std::vector<TComponents>&...>;

	public:
		View(ViewContainerType&& components, const SparseSet<Entity>& entities)
			: Components{ std::move(components) }
			, NrOfComponents{ std::get<0>(Components).size() }
			, Entities{ entities }
			, NrOfEntities{ entities.Size() }
		{
			assert(NrOfComponents == NrOfEntities);

			[this] <size_t ... Indices> (std::index_sequence<Indices...>)
			{
				(ComponentFlags[Indices] = ...) = (TComponents::GetComponentID(), ...);
			} (std::make_index_sequence<sizeof ... (TComponents)>{});
		}

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
			if (index < NrOfEntities)
			{
				bool b = (EntityManager::GetInstance()->GetEntitySignature(Entities[index]).test(ComponentFlags[Indices]) && ...);
				if (b)
				{
					auto tuple{ std::tuple<TComponents&...>(std::get<Indices>(Components)[index]...) };
					std::apply(function, tuple);
				}
			}
		}

		ViewContainerType Components;
		size_t NrOfComponents;
		SparseSet<Entity> Entities;
		size_t NrOfEntities;
		std::array<ComponentType, sizeof ... (TComponents)> ComponentFlags;
	};

	class System final
	{
	public:
		System() = default;
		~System();

		System(const System&) noexcept = delete;
		System(System&&) noexcept = delete;
		System& operator=(const System&) noexcept = delete;
		System& operator=(System&&) noexcept = delete;

		void AddEntity(Entity entity);
		Entity CreateEntity();

		void RemoveEntity(const Entity entity);
		void ClearEntities();
		void ReleaseEntities();

		size_t GetAmountOfEntities() const { return Entities.Size(); }

		template<typename TComponent>
		void AddComponent(const Entity entity) const
		{
			assert(entity != InvalidEntityID);
			assert(Entities.Contains(entity));

			ComponentManager::GetInstance()->AddComponent<TComponent>(entity);
			EntityManager::GetInstance()->SetEntitySignature(entity, TComponent::GetComponentID());
		}

		template<typename TComponent>
		void RemoveComponent(const Entity entity) const
		{
			assert(entity != InvalidEntityID);
			assert(Entities.Contains(entity));
			assert(EntityManager::GetInstance()->GetEntitySignature(entity).test(TComponent::GetComponentID()));

			ComponentManager::GetInstance()->RemoveComponent(entity, TComponent::GetComponentID());
			EntityManager::GetInstance()->GetEntitySignature(entity).flip(TComponent::GetComponentID());
		}

		template<typename ... TComponents>
		View<TComponents...> CreateView() const
		{
#ifdef _DEBUG
			const EntityManager* const pManager{ EntityManager::GetInstance() };
			for (const Entity entity : Entities)
			{
				assert(CheckEntityComponents<TComponents...>(entity));
			}
#endif
			return View<TComponents...>(std::tuple<std::vector<TComponents>&...>(ComponentManager::GetInstance()->GetComponents<TComponents>()...), Entities);
		}

		template<typename TComponent>
		TComponent& GetComponent(const Entity id) { assert(id != InvalidEntityID); return ComponentManager::GetInstance()->GetComponent<TComponent>(id); }
		template<typename TComponent>
		const TComponent& GetComponent(const Entity id) const { assert(id != InvalidEntityID); return ComponentManager::GetInstance()->GetComponent<TComponent>(id); }

		const SparseSet<Entity>& GetEntities() const { return Entities; }

	private:
		template<typename ... TComponents>
		bool CheckEntityComponents(const Entity entity) const
		{
			return (EntityManager::GetInstance()->GetEntitySignature(entity).test(TComponents::GetComponentID()) && ...);
		}

		SparseSet<Entity> Entities{};
	};
}
#endif