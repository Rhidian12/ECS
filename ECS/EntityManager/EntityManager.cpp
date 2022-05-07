#include "EntityManager.h"

namespace ECS
{
	EntityManager::EntityManager()
	{
		for (Entity i{}; i < MaxEntities; ++i)
		{
			Entities.push(i);
		}
	}

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
		assert(Entities.size() > 0);

		const Entity entity(Entities.front());
		Entities.pop();

		EntitySignatures.Add(entity);
		
		return entity;
	}

	void EntityManager::ReleaseEntity(Entity entity)
	{
	}

	EntitySignature EntityManager::GetEntitySignature(Entity entity) const
	{
		EntitySignatures.Find(entity);
	}
}