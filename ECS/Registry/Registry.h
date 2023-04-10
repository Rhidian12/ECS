#pragma once

#include "../ECSConstants.h"
#include "../SparseSet/SparseSet.h"
#include "../View/View.h"
#include "../ComponentIDGenerator/ComponentIDGenerator.h"
#include "../ComponentArray/ComponentArray.h"

#include "../Benchmark/BenchmarkUtils.h"

#include <assert.h> /* assert() */
#include <array> /* std::array */

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
		[[nodiscard]] View<Ts...> CreateView()
		{
			/* Get all components asked for by the user */
			std::tuple<ComponentArray<Ts>&...> comps
			{
				(*static_cast<ComponentArray<Ts>*>(GetComponentArray(GenerateComponentID<Ts>()).get()))...
			};

			return View<Ts...>{ std::move(comps), EntitySignatures };
		}

		template<typename T>
		T& AddComponent(const Entity entity)
		{
			SetEntitySignature(entity, GenerateComponentID<T>());

			std::unique_ptr<IComponentArray>& pool{ GetComponentArray(GenerateComponentID<T>()) };

			if (!pool)
			{
				pool.reset(new ComponentArray<T>{});
			}

			return static_cast<ComponentArray<T>*>(pool.get())->AddComponent(entity);
		}
		template<typename T, typename ... Ts>
		T& AddComponent(const Entity entity, Ts&& ... args)
		{
			SetEntitySignature(entity, GenerateComponentID<T>());

			std::unique_ptr<IComponentArray>& pool{ GetComponentArray(GenerateComponentID<T>()) };

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

			GetComponentArray(GenerateComponentID<T>())->Remove(entity);
			SetEntitySignature(entity, GenerateComponentID<T>(), false);
		}

		template<typename T>
		T& GetComponent(const Entity entity)
		{
			assert(GetComponentArray(GenerateComponentID<T>()));
			return static_cast<ComponentArray<T>*>(GetComponentArray(GenerateComponentID<T>()).get())->GetComponent(entity);
		}
		template<typename T>
		const T& GetComponent(const Entity entity) const
		{
			assert(GetComponentArray(GenerateComponentID<T>()));
			return static_cast<ComponentArray<T>*>(GetComponentArray(GenerateComponentID<T>()).get())->GetComponent(entity);
		}

		template<typename T>
		std::vector<T>& GetComponents()
		{
			assert(GetComponentArray(GenerateComponentID<T>()));
			return static_cast<ComponentArray<T>*>(GetComponentArray(GenerateComponentID<T>()).get())->GetComponents();
		}
		template<typename T>
		const std::vector<T>& GetComponents() const
		{
			assert(GetComponentArray(GenerateComponentID<T>()));
			return static_cast<ComponentArray<T>*>(GetComponentArray(GenerateComponentID<T>()).get())->GetComponents();
		}

		Entity CreateEntity();
		bool ReleaseEntity(const Entity entity);
		size_t GetAmountOfEntities() const
		{
			return Entities.Size();
		}

		void Clear();

		void SetEntitySignature(const Entity entity, const ComponentType id, const bool val = true);
		const EntitySignature& GetEntitySignature(const Entity entity) const;

	private:
		void RemoveAllComponents(const Entity entity, const EntitySignature& sig);
		bool HasEntity(const Entity entity) const;
		std::unique_ptr<IComponentArray>& GetComponentArray(const ComponentType cType);

		// Entities
		std::vector<std::pair<Entity, EntitySignature>> EntitySignatures; // [TODO]: Make a map that uses arrays 
		SparseSet<Entity> Entities;
		std::vector<Entity> RecycledEntities;
		Entity CurrentEntityCounter;

		// Components
		std::vector<std::pair<ComponentType, std::unique_ptr<IComponentArray>>> ComponentPools; // [TODO]: Make a map that uses arrays 
	};
}