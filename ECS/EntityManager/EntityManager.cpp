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

		if (EntitySignatures.size() <= entity)
		{
			EntitySignatures.resize(entity + 1);
		}

		EntitySignatures[entity] = EntitySignature();;
		
		return entity;
	}

	void EntityManager::ReleaseEntity(Entity entity)
	{
		assert(entity < EntitySignatures.size());
		
		EntitySignatures.erase(EntitySignatures.begin() + entity);

		EntitySignatures.push_back(entity);
	}
}