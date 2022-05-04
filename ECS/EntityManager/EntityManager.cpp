#include "EntityManager.h"

namespace ECS
{
	EntityManager* EntityManager::GetInstance()
	{
		if (!Instance)
		{
			Instance = std::make_unique<EntityManager>();
		}

		return Instance.get();
	}

	Entity EntityManager::CreateEntity()
	{
		assert(static_cast<Entity>(Entities.size()) < MaxEntities);

		EntitySignatures.push_back(EntitySignature{});
		Entities.push_back(static_cast<Entity>(Entities.size()));
		return Entities.back();
	}
}