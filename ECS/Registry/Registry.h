#pragma once

#include "../ECSConstants.h"
#include "../SparseSet/SparseSet.h"
#include "../View/View.h"
#include "../ComponentManager/ComponentManager.h"
#include "../ComponentIDGenerator/ComponentIDGenerator.h"

#include <assert.h> /* assert() */
#include <unordered_map> /* unordered_map */

namespace ECS
{
	class Registry final
	{
	public:
		Registry();

		Registry(const Registry&) noexcept = delete;
		Registry(Registry&& other) noexcept;
		Registry& operator=(const Registry&) noexcept = delete;
		Registry& operator=(Registry&& other) noexcept;

		template<typename ... TComponents>
		[[nodiscard]] View<TComponents...> CreateView() const
		{
			return View<TComponents...>(std::tuple<std::vector<TComponents>&...>(ComponentManager::GetInstance().GetComponents<TComponents>()...));
		}

		template<typename T>
		T AddComponent(const Entity entity)
		{
			SetEntitySignature(entity, GenerateComponentID<T>());
			return ComponentManager::GetInstance().AddComponent<T>(entity);
		}
		/* [TODO]: Add variadic template overload */

		template<typename T>
		void RemoveComponent(const Entity entity)
		{
			assert(Entities.Contains(entity));

			ComponentManager::GetInstance().RemoveComponent(entity);
			SetEntitySignature(entity, GenerateComponentID<T>(), false);
		}

		template<typename T>
		T& GetComponent(const Entity entity)
		{
			return ComponentManager::GetInstance().GetComponent<T>(entity);
		}
		template<typename T>
		const T& GetComponent(const Entity entity) const
		{
			return ComponentManager::GetInstance().GetComponent<T>(entity);
		}

		Entity CreateEntity();
		bool ReleaseEntity(Entity entity);
		Entity GetAmountOfEntities() const { return CurrentEntityCounter; }

		void SetEntitySignature(const Entity entity, const EntitySignature sig) { assert(EntitySignatures.find(entity) != EntitySignatures.cend()); EntitySignatures[entity] = sig; }
		void SetEntitySignature(const Entity entity, const ComponentType id, const bool val = true) { assert(EntitySignatures.find(entity) != EntitySignatures.cend()); EntitySignatures[entity].set(id, val); }

		EntitySignature GetEntitySignature(const Entity entity) const { assert(EntitySignatures.find(entity) != EntitySignatures.cend()); return EntitySignatures.find(entity)->second; }

	private:
		std::unordered_map<Entity, EntitySignature> EntitySignatures;
		SparseSet<Entity> Entities;
		Entity CurrentEntityCounter;
	};
}