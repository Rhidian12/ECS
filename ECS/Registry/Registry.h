#pragma once

#include "../ECSConstants.h"
#include "../SparseSet/SparseSet.h"
#include "../View/View.h"
#include "../ComponentIDGenerator/ComponentIDGenerator.h"
#include "../ComponentArray/ComponentArray.h"
#include "../Memory/Memory.h"

#include <assert.h> /* assert() */
#include <unordered_map> /* unordered_map */
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

		template<typename ... TComponents>
		[[nodiscard]] View<TComponents...> CreateView()
		{
			/* Get all components asked for by the user */
			std::tuple<std::vector<std::reference_wrapper<TComponents>, STLAllocator<std::reference_wrapper<TComponents>, StackAllocator>>...> comps
			{
				std::vector<std::reference_wrapper<TComponents>, STLAllocator<std::reference_wrapper<TComponents>, StackAllocator>>
				{
					GetComponents<TComponents>().begin(), GetComponents<TComponents>().end(), Allocator
				}...
			};

			std::array<std::vector<Entity>*, sizeof ... (TComponents)> ents{};
			FillArray<TComponents...>(ents, std::make_index_sequence<sizeof ... (TComponents)>{});

			/* Loop over a vector and get rid of the elements in the vector that are attached to an entity but does not have all components */
			// FilterVector<TComponents...>(comps, std::make_index_sequence<sizeof ... (TComponents)>{});

			return View<TComponents...>{ std::move(comps), std::move(ents), EntitySignatures };
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

			T& comp{ static_cast<ComponentArray<T>*>(pool.get())->AddComponent(entity) };

			if (static_cast<ComponentArray<T>*>(pool.get())->GetComponents().size() * sizeof(T) >
				Allocator.GetCapacity() * 2.f / 3.f)
			{
				Allocator.Reallocate(Allocator.GetCapacity() * 2);
			}

			return comp;
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

			T& comp{ static_cast<ComponentArray<T>*>(pool.get())->AddComponent<Ts...>(entity, std::forward<Ts>(args)...) };

			if (static_cast<ComponentArray<T>*>(pool.get())->GetComponents().size() * sizeof(T) > 
				Allocator.GetCapacity() * 2.f / 3.f)
			{
				Allocator.Reallocate(Allocator.GetCapacity() * 2);
			}

			return comp;
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

		const EntitySignature& GetEntitySignature(const Entity entity) const { assert(EntitySignatures.find(entity) != EntitySignatures.cend()); return EntitySignatures.find(entity)->second; }

	private:
		void RemoveAllComponents(const Entity entity, const EntitySignature& sig);

		template<typename ... TComponents, size_t ... Indices>
		void FilterVector(std::tuple<std::vector<std::reference_wrapper<TComponents>, STLAllocator<std::reference_wrapper<TComponents>, StackAllocator>>...>& tuple,
			std::index_sequence<Indices...>)
		{
			Entity val{};
			for (Entity entity : Entities)
			{
				const EntitySignature& sig{ GetEntitySignature(entity) };

				if (!(sig.test(GenerateComponentID<TComponents>()) && ...))
				{
					entity -= val;
					(SafeRemove(std::get<Indices>(tuple), entity), ...);
					val += entity;
				}
			}
		}

		template<typename T>
		void SafeRemove(std::vector<std::reference_wrapper<T>, STLAllocator<std::reference_wrapper<T>, StackAllocator>>& v, const Entity entity)
		{
			if (v.cbegin() + entity < v.cend())
			{
				v.erase(v.begin() + entity);
			}
		}

		template<typename ... Ts, size_t ... Is>
		void FillArray(std::array<std::vector<Entity>*, sizeof ... (Ts)>& arr, std::index_sequence<Is...>)
		{
			((arr[Is] = &static_cast<ComponentArray<Ts>*>(ComponentPools[GenerateComponentID<Ts>()].get())->GetKeys()), ...);
		}

		/* [TODO]: Sort Entities based on their EntitySignature 
		Keep pointers to each start and end entry of that sort value 
		so we can make our loop smaller instead of Max Entities */
		std::unordered_map<Entity, EntitySignature> EntitySignatures;
		SparseSet<Entity> Entities;
		Entity CurrentEntityCounter;
		std::unordered_map<ComponentType, std::unique_ptr<IComponentArray>> ComponentPools;
		StackAllocator Allocator;
	};
}