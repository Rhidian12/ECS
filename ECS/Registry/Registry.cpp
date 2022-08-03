#include "Registry.h"

#include "../ComponentManager/ComponentManager.h"

namespace ECS
{
	Registry::Registry()
		: EntitySignatures{}
		, Entities{}
		, CurrentEntityCounter{}
	{}

	Entity Registry::CreateEntity()
	{
		assert(CurrentEntityCounter <= MaxEntities);

		const Entity entity(CurrentEntityCounter++);
		Entities.Add(entity);

		EntitySignatures.insert(std::make_pair(entity, EntitySignature{}));

		return entity;
	}

	bool Registry::ReleaseEntity(Entity entity)
	{
		if (Entities.Contains(entity))
		{
			ComponentManager::GetInstance().RemoveAllComponents(entity, GetEntitySignature(entity));

			EntitySignatures.erase(entity);

			Entities.Remove(entity);

			--CurrentEntityCounter;

			return true;
		}

		return false;
	}
}