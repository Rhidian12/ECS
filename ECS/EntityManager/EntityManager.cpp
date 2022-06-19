#include "EntityManager.h"

#include "../ComponentManager/ComponentManager.h"

namespace ECS
{
	EntityManager::EntityManager()
		: CurrentEntityCounter{}
	{
		for (Entity i{}; i < MaxEntities; ++i)
		{
			Entities.Add(i);
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
		assert(Entities.Size() > 0);

		const Entity entity(Entities.Find(CurrentEntityCounter++));
		Entities.Remove(entity);

		if (EntitySignatures.size() <= entity)
		{
			EntitySignatures.resize(entity + 1);
		}

		EntitySignatures[entity] = EntitySignature();

		return entity;
	}

	bool EntityManager::ReleaseEntity(Entity entity)
	{
		if (!Entities.Contains(entity))
		{
			assert(entity < EntitySignatures.size());

			ComponentManager::GetInstance()->RemoveComponent(entity, EntityManager::GetInstance()->GetEntitySignature(entity));

			EntitySignatures.erase(EntitySignatures.begin() + entity);
			EntitySignatures.push_back(EntitySignature());

			Entities.Add(entity);

			--CurrentEntityCounter;

			return true;
		}

		return false;
	}
}