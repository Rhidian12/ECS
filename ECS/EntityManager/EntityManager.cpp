#include "EntityManager.h"

#include "../ComponentManager/ComponentManager.h"

namespace ECS
{
	EntityManager::EntityManager()
		: EntitySignatures{}
		, EntitySignatureIndices{}
		, Entities{}
		, CurrentEntityCounter{}
	{
		for (Entity i{}; i < MaxEntities; ++i)
		{
			Entities.Add(i);
		}

		EntitySignatureIndices.reserve(MaxEntities);
		EntitySignatureIndices.resize(MaxEntities, InvalidEntityID);
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
		assert(Entities.Contains(CurrentEntityCounter));

		const Entity entity(CurrentEntityCounter++);
		Entities.Remove(entity);

		EntitySignatureIndices[entity] = static_cast<Entity>(EntitySignatures.size());
		EntitySignatures.push_back(EntitySignature());

		return entity;
	}

	bool EntityManager::ReleaseEntity(Entity entity)
	{
		if (!Entities.Contains(entity))
		{
			ComponentManager::GetInstance()->RemoveAllComponents(entity, GetEntitySignature(entity));

			EntitySignatures.erase(EntitySignatures.begin() + EntitySignatureIndices[entity]);
			EntitySignatureIndices[entity] = InvalidEntityID;

			for (Entity i(entity + static_cast<Entity>(1u)); i < EntitySignatureIndices.size(); ++i)
			{
				if (EntitySignatureIndices[i] != 0 && EntitySignatureIndices[i] != InvalidEntityID)
				{
					--EntitySignatureIndices[i];
				}
			}

			Entities.Add(entity);

			--CurrentEntityCounter;

			return true;
		}

		return false;
	}
}