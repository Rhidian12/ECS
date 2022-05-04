#pragma once

#include "../ECSConstants.h"

#include <vector>
#include <assert.h>
#include <memory>

namespace ECS
{
	class EntityManager final
	{
	public:
		static EntityManager* GetInstance()
		{
			if (!Instance)
			{
				Instance = std::make_unique<EntityManager>();
			}

			return Instance.get();
		}

		EntityManager(const EntityManager&) noexcept = delete;
		EntityManager(EntityManager&&) noexcept = delete;
		EntityManager& operator=(const EntityManager&) noexcept = delete;
		EntityManager& operator=(EntityManager&&) noexcept = delete;

		Entity CreateEntity();

		void SetEntitySignature(Entity entity, ComponentType sig) { assert(entity < EntitySignatures.size()); EntitySignatures[entity].set(sig); }
		
		const EntitySignature& GetEntitySignature(Entity entity) const { assert(entity < EntitySignatures.size()); return EntitySignatures[entity]; }

	private:
		EntityManager() = default;

		friend std::unique_ptr<EntityManager> std::make_unique();

		inline static std::unique_ptr<EntityManager> Instance;

		std::vector<EntitySignature> EntitySignatures;
		std::vector<Entity> Entities;
	};
}