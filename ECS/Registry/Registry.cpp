#include "Registry.h"

#include "../ComponentManager/ComponentManager.h"

#include <iostream>
namespace ECS
{
	Registry::Registry()
		: EntitySignatures{}
		, Entities{}
		, CurrentEntityCounter{}
	{}

	Registry::~Registry()
	{
		for (int i{ static_cast<int>(Entities.Size()) - 1 }; i >= 0; --i)
		{
			ReleaseEntity(Entities[i]);
		}
	}

	Registry::Registry(Registry&& other) noexcept
		: EntitySignatures{ std::move(other.EntitySignatures) }
		, Entities{ std::move(other.Entities) }
		, CurrentEntityCounter{ std::move(other.CurrentEntityCounter) }
		, ComponentPools{ std::move(other.ComponentPools) }
	{
		other.EntitySignatures.clear();
		other.Entities.Clear();
		other.CurrentEntityCounter = 0;
		other.ComponentPools.clear();
	}

	Registry& Registry::operator=(Registry&& other) noexcept
	{
		EntitySignatures = std::move(other.EntitySignatures);
		Entities = std::move(other.Entities);
		CurrentEntityCounter = std::move(other.CurrentEntityCounter);
		ComponentPools = std::move(other.ComponentPools);

		other.EntitySignatures.clear();
		other.Entities.Clear();
		other.CurrentEntityCounter = 0;
		other.ComponentPools.clear();

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
			RemoveAllComponents(entity, GetEntitySignature(entity));

			EntitySignatures.erase(entity);

			Entities.Remove(entity);

			--CurrentEntityCounter;

			return true;
		}

		return false;
	}

	void Registry::RemoveAllComponents(const Entity entity, const EntitySignature& sig)
	{
		for (ComponentType i{}; i < MaxComponentTypes; ++i)
		{
			if (sig.test(i))
			{
				assert(ComponentPools[i]);

				ComponentPools[i]->Remove(Entities.GetIndex(entity));
			}
		}
	}
}