#pragma once

#include "../ECSConstants.h"
#include "../SparseSet/SparseSet.h"

#include <vector> /* std::vector */
#include <assert.h> /* assert() */
#include <memory> /* std::unique_ptr */
#include <queue> /* std::queue */

namespace ECS
{
	namespace Internal
	{
		inline Entity ConvertToEntity(EntitySignature sig) { return static_cast<Entity>(sig.to_ulong()); }
	}

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
		void SetEntitySignature(Entity entity, EntitySignature sig);

	private:
		EntityManager();

		friend std::unique_ptr<EntityManager> std::make_unique();

		inline static std::unique_ptr<EntityManager> Instance;

		SparseSet<Entity, EntitySignature, decltype(Internal::ConvertToEntity)> EntitySignatures{ &Internal::ConvertToEntity };
		std::queue<Entity> Entities;
	};
}