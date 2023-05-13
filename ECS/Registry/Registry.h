#pragma once

#include "../ECSConstants.h"
#include "../ComponentArray/ComponentArray.h"
#include "../ComponentIDGenerator/ComponentIDGenerator.h"
#include "../View/View.h"
#include "../SparseSet/SparseSet.h"

#include <assert.h> /* assert() */
#include <queue>

namespace ECS
{
	class Registry final
	{
	public:
		Registry();
		~Registry();

		Registry(const Registry&) noexcept = delete;
		Registry(Registry&& other) noexcept;
		Registry& operator=(const Registry&) noexcept = delete;
		Registry& operator=(Registry&& other) noexcept;

		template<typename ... Ts>
		[[nodiscard]] View<Ts...> CreateView() const
		{
			/* Get all components asked for by the user */
			std::tuple<ComponentArray<Ts>&...> comps
			{
				(*static_cast<ComponentArray<Ts>*>(GetComponentArray(ECS::GenerateComponentID<Ts>()).get()))...
			};

			return View<Ts...>(std::move(comps), Entities);
		}

		template<typename T>
		T& AddComponent(const Entity entity)
		{
			std::unique_ptr<IComponentArray>& pool{ GetComponentArray(ECS::GenerateComponentID<T>()) };

			if (!pool)
			{
				pool.reset(new ComponentArray<T>{});
			}

			return static_cast<ComponentArray<T>*>(pool.get())->AddComponent(entity);
		}
		template<typename T, typename ... Ts>
		T& AddComponent(const Entity entity, Ts&& ... args)
		{
			std::unique_ptr<IComponentArray>& pool{ GetComponentArray(ECS::GenerateComponentID<T>()) };

			if (!pool)
			{
				pool.reset(new ComponentArray<T>{});
			}

			return static_cast<ComponentArray<T>*>(pool.get())->AddComponent<Ts...>(entity, std::forward<Ts>(args)...);
		}

		template<typename T>
		void RemoveComponent(const Entity entity)
		{
			assert(HasEntity(entity));

			GetComponentArray(ECS::GenerateComponentID<T>())->Remove(entity);
		}

		template<typename T>
		[[nodiscard]] bool HasComponent(const Entity entity) const
		{
			return static_cast<ComponentArray<T>*>(GetComponentArray(ECS::GenerateComponentID<T>()).get())->HasEntity(entity);
		}

		template<typename T>
		[[nodiscard]] T& GetComponent(const Entity entity)
		{
			assert(GetComponentArray(ECS::GenerateComponentID<T>()));
			return static_cast<ComponentArray<T>*>(GetComponentArray(ECS::GenerateComponentID<T>()).get())->GetComponent(entity);
		}
		template<typename T>
		[[nodiscard]] const T& GetComponent(const Entity entity) const
		{
			assert(GetComponentArray(ECS::GenerateComponentID<T>()));
			return static_cast<ComponentArray<T>*>(GetComponentArray(ECS::GenerateComponentID<T>()).get())->GetComponent(entity);
		}

		template<typename T>
		[[nodiscard]] Entity FindEntity(const T& comp)
		{
			assert(GetComponentArray(ECS::GenerateComponentID<T>()));
			return static_cast<ComponentArray<T>*>(GetComponentArray(ECS::GenerateComponentID<T>()).get())->FindEntity(comp);
		}

		[[nodiscard]] Entity CreateEntity();
		[[nodiscard]] size_t GetAmountOfEntities() const { return Entities.Size(); }
		[[nodiscard]] bool HasEntity(const Entity entity) const;
		bool ReleaseEntity(const Entity entity);

		template<typename ... Ts>
		[[nodiscard]] bool CanViewBeCreated() const
		{
			return[this]<size_t ... Is>(std::index_sequence<Is...>)->bool
			{
				return ((std::find_if(ComponentPools.cbegin(), ComponentPools.cend(), [](const auto& cPool)->bool
					{
						return ECS::GenerateComponentID<Ts>() == cPool.first;
					}) != ComponentPools.cend()) && ...);

			}(std::make_index_sequence<sizeof ... (Ts)>{});
		}

		void Clear();

	private:
		void RemoveAllComponents(const Entity entity);
		[[nodiscard]] std::unique_ptr<IComponentArray>& GetComponentArray(const size_t cType);
		[[nodiscard]] const std::unique_ptr<IComponentArray>& GetComponentArray(const size_t cType) const;

		// Entities
		SparseSet<Entity> Entities;
		std::queue<Entity> RecycledEntities;
		Entity CurrentEntityCounter;

		// Components
		std::vector<std::pair<size_t, std::unique_ptr<IComponentArray>>> ComponentPools; // [TODO]: Make a map that uses arrays 
	};
}