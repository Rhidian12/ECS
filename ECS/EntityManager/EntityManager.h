#pragma once

#include "../ECSConstants.h"
#include "../SparseSet/SparseSet.h"

#include <vector> /* std::vector */
#include <assert.h> /* assert() */
#include <memory> /* std::unique_ptr */
#include <deque> /* std::deque */

namespace ECS
{
	class EntityManager final
	{
	public:
		static EntityManager* GetInstance();

		EntityManager(const EntityManager&) noexcept = delete;
		EntityManager(EntityManager&&) noexcept = delete;
		EntityManager& operator=(const EntityManager&) noexcept = delete;
		EntityManager& operator=(EntityManager&&) noexcept = delete;

		Entity CreateEntity();
		bool ReleaseEntity(Entity entity);

		void SetEntitySignature(Entity entity, const EntitySignature sig) { assert(entity < EntitySignatures.size()); EntitySignatures[entity] = sig; }
		void SetEntitySignature(Entity entity, const ComponentType id) { assert(entity < EntitySignatures.size()); EntitySignatures[entity].set(id); }

		EntitySignature& GetEntitySignature(const Entity entity) { assert(EntitySignatureIndices[entity] != InvalidEntityID); return EntitySignatures[EntitySignatureIndices[entity]]; }
		EntitySignature GetEntitySignature(const Entity entity) const { assert(EntitySignatureIndices[entity] != InvalidEntityID); return EntitySignatures[EntitySignatureIndices[entity]]; }

	private:
		EntityManager();

		friend std::unique_ptr<EntityManager> std::make_unique();

		inline static std::unique_ptr<EntityManager> Instance;

		std::vector<EntitySignature> EntitySignatures;
		std::vector<Entity> EntitySignatureIndices;
		SparseSet<Entity> Entities;
		Entity CurrentEntityCounter;
	};
}