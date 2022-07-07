#pragma once

#include "../ECSConstants.h"
#include "../SparseSet/SparseSet.h"
#include "../View/View.h"

#include <vector> /* std::vector */
#include <assert.h> /* assert() */
#include <memory> /* std::unique_ptr */
#include <deque> /* std::deque */

namespace ECS
{
	class EntityManager final
	{
	public:
		EntityManager();

		/* [TODO]: Rule of 5 */

		template<typename ... TComponents>
		[[nodiscard]] View<TComponents...> CreateView() const
		{
			return View<TComponents...>(std::tuple<std::vector<TComponents>&...>(ComponentManager::GetInstance().GetComponents<TComponents>()...));
		}

		template<typename T>
		T AddComponent(const Entity entity)
		{
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

		void SetEntitySignature(const Entity entity, const EntitySignature sig) { assert(entity < EntitySignatures.size()); EntitySignatures[entity] = sig; }
		void SetEntitySignature(const Entity entity, const ComponentType id, const bool val = true) { assert(entity < EntitySignatures.size()); EntitySignatures[entity].set(id, val); }

		EntitySignature& GetEntitySignature(const Entity entity) { assert(EntitySignatureIndices[entity] != InvalidEntityID); return EntitySignatures[EntitySignatureIndices[entity]]; }
		EntitySignature GetEntitySignature(const Entity entity) const { assert(EntitySignatureIndices[entity] != InvalidEntityID); return EntitySignatures[EntitySignatureIndices[entity]]; }

	private:
		std::vector<EntitySignature> EntitySignatures;
		std::vector<Entity> EntitySignatureIndices;
		SparseSet<Entity> Entities;
		Entity CurrentEntityCounter;
	};
}