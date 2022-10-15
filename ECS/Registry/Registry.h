#pragma once

#include "../ECSConstants.h"
#include "../SparseSet/SparseSet.h"
#include "../View/View.h"
#include "../ComponentIDGenerator/ComponentIDGenerator.h"
#include "../ComponentArray/ComponentArray.h"

#include <assert.h> /* assert() */
#include <unordered_map> /* unordered_map */
#include <array> /* std::array */

namespace ECS
{
	class Registry final
	{
	public:
		Registry(const size_t nrOfEntitiesPerList = 1000);
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
				(*static_cast<ComponentArray<Ts>*>(ComponentPools[GenerateComponentID<Ts>()].get()))...
			};

			return View<Ts...>{ std::move(comps), EntitySignatures };
		}

		template<typename T>
		T& AddComponent(const Entity entity)
		{
			SetEntitySignature(entity, GenerateComponentID<T>());

			std::unique_ptr<IComponentArray>& pool{ ComponentPools[GenerateComponentID<T>()] };

			if (!pool)
			{
				pool.reset(new ComponentArray<T>{ NrOfEntitiesPerList });
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
				pool.reset(new ComponentArray<T>{ NrOfEntitiesPerList });
			}

			return static_cast<ComponentArray<T>*>(pool.get())->AddComponent<Ts...>(entity, std::forward<Ts>(args)...);
		}

		template<typename T>
		void RemoveComponent(const Entity entity)
		{
			assert(HasEntity(entity));

			ComponentPools[GenerateComponentID<T>()]->Remove(entity);
			SetEntitySignature(entity, GenerateComponentID<T>(), false);
		}

		template<typename T>
		T& GetComponent(const Entity entity)
		{
			assert(ComponentPools[GenerateComponentID<T>()]);
			return static_cast<ComponentArray<T>*>(ComponentPools[GenerateComponentID<T>()].get())->GetComponent(entity);
		}
		template<typename T>
		const T& GetComponent(const Entity entity) const
		{
			assert(ComponentPools[GenerateComponentID<T>()]);
			return static_cast<ComponentArray<T>*>(ComponentPools[GenerateComponentID<T>()].get())->GetComponent(entity);
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
		bool HasEntity(const Entity entity) const;

		std::unordered_map<Entity, EntitySignature> EntitySignatures;
		std::vector<std::vector<Entity>> Entities;
		Entity CurrentEntityCounter;
		std::unordered_map<ComponentType, std::unique_ptr<IComponentArray>> ComponentPools;
		size_t NrOfEntitiesPerList;
	};
}