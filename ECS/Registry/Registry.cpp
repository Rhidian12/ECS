#include "Registry.h"

#include "../ComponentManager/ComponentManager.h"

namespace ECS
{
	Registry::Registry()
		: EntitySignatures{}
		// , EntitySignatureIndices{}
		, Entities{}
		, CurrentEntityCounter{}
	{
		for (Entity i{}; i < MaxEntities; ++i)
		{
			Entities.Add(i);
		}

		// EntitySignatureIndices.reserve(MaxEntities);
		// EntitySignatureIndices.resize(MaxEntities, InvalidEntityID);

		// EntitySignatures.reserve(MaxEntities);
		// EntitySignatures.resize(MaxEntities, EntitySignature());
	}

	Entity Registry::CreateEntity()
	{
		assert(Entities.Size() > 0);
		assert(Entities.Contains(CurrentEntityCounter));

		const Entity entity(CurrentEntityCounter++);
		Entities.Remove(entity);

		// EntitySignatureIndices[entity] = static_cast<Entity>(CurrentEntityCounter++);
		EntitySignatures.insert(std::make_pair(entity, EntitySignature{}));

		return entity;
	}

	bool Registry::ReleaseEntity(Entity entity)
	{
		if (!Entities.Contains(entity))
		{
			ComponentManager::GetInstance().RemoveAllComponents(entity, GetEntitySignature(entity));

			// EntitySignatures[EntitySignatureIndices[entity]] = EntitySignature();
			// EntitySignatureIndices[entity] = InvalidEntityID;
			EntitySignatures.erase(entity);

			Entities.Add(entity);

			--CurrentEntityCounter;

			return true;
		}

		return false;
	}
}