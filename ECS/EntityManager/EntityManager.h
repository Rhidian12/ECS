#pragma once

#include "../ECSConstants.h"
#include "../SparseSet/SparseSet.h"

#include <vector> /* std::vector */
#include <assert.h> /* assert() */
#include <memory> /* std::unique_ptr */
#include <queue> /* std::queue */

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
		void ReleaseEntity(Entity entity);

		EntitySignature GetEntitySignature(Entity entity) const;

	private:
		EntityManager();

		friend std::unique_ptr<EntityManager> std::make_unique();

		inline static std::unique_ptr<EntityManager> Instance;

		SparseSet<EntitySignature> EntitySignatures;
		std::queue<Entity> Entities;
	};
}