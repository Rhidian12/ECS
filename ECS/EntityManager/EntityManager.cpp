#include "EntityManager.h"

namespace ECS
{
	EntityManager::EntityManager()
	{
		for (Entity i{}; i < MaxEntities; ++i)
		{
			Entities.push_back(i);
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
		Entities.pop_front();

		if (EntitySignatures.size() <= entity)
		{
			EntitySignatures.resize(entity + 1);
		}

		EntitySignatures[entity] = EntitySignature();;

		return entity;
	}

	bool EntityManager::ReleaseEntity(Entity entity)
	{
		if (std::find(Entities.cbegin(), Entities.cend(), entity) == Entities.cend())
		{
			assert(entity < EntitySignatures.size());

			EntitySignatures.erase(EntitySignatures.begin() + entity);

			EntitySignatures.push_back(entity);

			Entities.push_back(entity);

			return true;
		}

		return false;
	}
}