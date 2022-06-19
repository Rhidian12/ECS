#include "EntityManager.h"

namespace ECS
{
	EntityManager::EntityManager()
		: CurrentEntityCounter{}
	{
		for (Entity i{}; i < MaxEntities; ++i)
		{
			// Entities.push_back(i);
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
		//assert(Entities.size() > 0);

		//const Entity entity(Entities.front());
		//Entities.pop_front();

		//if (EntitySignatures.size() <= entity)
		//{
		//	EntitySignatures.resize(entity + 1);
		//}

		//EntitySignatures[entity] = EntitySignature();

		//return entity;

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
		//if (std::find(Entities.cbegin(), Entities.cend(), entity) == Entities.cend())
		//{
		//	assert(entity < EntitySignatures.size());

		//	EntitySignatures.erase(EntitySignatures.begin() + entity);

		//	EntitySignatures.push_back(entity);

		//	Entities.push_back(entity);

		//	return true;
		//}

		//return false;

		if (!Entities.Contains(entity))
		{
			assert(entity < EntitySignatures.size());

			EntitySignatures.erase(EntitySignatures.begin() + entity);

			EntitySignatures.push_back(entity);

			Entities.Add(entity);

			--CurrentEntityCounter;

			return true;
		}

		return false;
	}
}