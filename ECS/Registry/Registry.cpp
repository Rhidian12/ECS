#include "Registry.h"

#include "../ComponentManager/ComponentManager.h"

namespace ECS
{
	Registry::Registry()
		: EntitySignatures{}
		, Entities{}
		, CurrentEntityCounter{}
	{}

	Registry::~Registry()
	{
		for (const Entity entity : Entities)
		{
			ReleaseEntity(entity);
		}
	}

	Registry::Registry(Registry&& other) noexcept
		: EntitySignatures{ std::move(other.EntitySignatures) }
		, Entities{ std::move(other.Entities) }
		, CurrentEntityCounter{ std::move(other.CurrentEntityCounter) }
	{
		other.EntitySignatures.clear();
		other.Entities.Clear();
		other.CurrentEntityCounter = 0;
	}

	Registry& Registry::operator=(Registry&& other) noexcept
	{
		EntitySignatures = std::move(other.EntitySignatures);
		Entities = std::move(other.Entities);
		CurrentEntityCounter = std::move(other.CurrentEntityCounter);

		other.EntitySignatures.clear();
		other.Entities.Clear();
		other.CurrentEntityCounter = 0;

		return *this;
	}

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