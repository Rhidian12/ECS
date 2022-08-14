#pragma once

#include "../ECSConstants.h"
#include "../SparseSet/SparseSet.h"
#include "../View/View.h"
#include "../ComponentIDGenerator/ComponentIDGenerator.h"
#include "../ComponentArray/ComponentArray.h"

#include <assert.h> /* assert() */
#include <unordered_map> /* unordered_map */

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

		template<typename ... TComponents>
		[[nodiscard]] View<TComponents...> CreateView()
		{
			// commented code is wrong but runs at least
			// return View<TComponents...>{ std::tuple<std::vector<TComponents>&...>{ GetComponents<TComponents>()... } };

			using TTuple = std::tuple<std::vector<std::reference_wrapper<TComponents>>...>;

			/* Get all components asked for by the user */
			TTuple comps{ std::vector<std::reference_wrapper<TComponents>>{ GetComponents<TComponents>().begin(), GetComponents<TComponents>().end() }...};

			/* Remove any component that is attached to an entity that does NOT have the other components as well */

			/* Loop over a vector and get rid of the elements in the vector that are attached to an entity but does not have all components */
			auto indexSequence{ std::make_index_sequence<sizeof ... (TComponents)>{} };
			FilterVector<TComponents...>(comps, indexSequence);

			return View<TComponents...>{ std::move(comps) };
		}

		template<typename T>
		T& AddComponent(const Entity entity)
		{
			SetEntitySignature(entity, GenerateComponentID<T>());

			std::unique_ptr<IComponentArray>& pool{ ComponentPools[GenerateComponentID<T>()] };

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

			std::unique_ptr<IComponentArray>& pool{ ComponentPools[GenerateComponentID<T>()] };

			if (!pool)
			{
				pool.reset(new ComponentArray<T>{});
			}

			return static_cast<ComponentArray<T>*>(pool.get())->AddComponent<Ts...>(entity, std::forward<Ts>(args)...);
		}

		template<typename T>
		void RemoveComponent(const Entity entity)
		{
			assert(Entities.Contains(entity));

			ComponentPools[GenerateComponentID<T>()]->Remove(entity);
			SetEntitySignature(entity, GenerateComponentID<T>(), false);
		}

		template<typename T>
		T& GetComponent(const Entity entity)
		{
			assert(ComponentPools[GenerateComponentID<T>()]);
			return static_cast<ComponentArray<T>*>(ComponentPools[GenerateComponentID<T>()].get())->GetComponent<T>(entity);
		}
		template<typename T>
		const T& GetComponent(const Entity entity) const
		{
			assert(ComponentPools[GenerateComponentID<T>()]);
			return static_cast<ComponentArray<T>*>(ComponentPools[GenerateComponentID<T>()].get())->GetComponent<T>(entity);
		}

		template<typename T>
		std::vector<T>& GetComponents()
		{
			assert(ComponentPools[GenerateComponentID<T>()]);
			return static_cast<ComponentArray<T>*>(ComponentPools[GenerateComponentID<T>()].get())->GetComponents();
		}
		template<typename T>
		const std::vector<T>& GetComponents() const
		{
			assert(ComponentPools[GenerateComponentID<T>()]);
			return static_cast<ComponentArray<T>*>(ComponentPools[GenerateComponentID<T>()].get())->GetComponents();
		}

		Entity CreateEntity();
		bool ReleaseEntity(Entity entity);
		Entity GetAmountOfEntities() const { return CurrentEntityCounter; }

		void SetEntitySignature(const Entity entity, const EntitySignature sig) { assert(EntitySignatures.find(entity) != EntitySignatures.cend()); EntitySignatures[entity] = sig; }
		void SetEntitySignature(const Entity entity, const ComponentType id, const bool val = true) { assert(EntitySignatures.find(entity) != EntitySignatures.cend()); EntitySignatures[entity].set(id, val); }

		EntitySignature GetEntitySignature(const Entity entity) const { assert(EntitySignatures.find(entity) != EntitySignatures.cend()); return EntitySignatures.find(entity)->second; }

	private:
		void RemoveAllComponents(const Entity entity, const EntitySignature& sig);

		template<typename ... TComponents, size_t ... Indices>
		void FilterVector(std::tuple<std::vector<std::reference_wrapper<TComponents>>...>& tuple, std::index_sequence<Indices...>)
		{
			IComponentArray* compArrays[sizeof ... (TComponents)]{ ComponentPools[GenerateComponentID<TComponents>()].get() ... };

			for (const Entity entity : Entities)
			{
				if (!(static_cast<ComponentArray<TComponents>*>(compArrays[Indices])->HasComponent(entity) && ...))
				{
					(SafeRemoveComponent(entity, compArrays[Indices], std::get<Indices>(tuple)), ...);
				}
			}
		}

		template<typename T>
		void SafeRemoveComponent(const Entity entity, IComponentArray* pCompArr, std::vector<std::reference_wrapper<T>>& v)
		{
			if (static_cast<ComponentArray<T>*>(pCompArr)->HasComponent(entity))
			{
				v.erase(v.begin() + entity);
			}
		}

		std::unordered_map<Entity, EntitySignature> EntitySignatures;
		SparseSet<Entity> Entities;
		Entity CurrentEntityCounter;
		std::unordered_map<ComponentType, std::unique_ptr<IComponentArray>> ComponentPools;
	};
}