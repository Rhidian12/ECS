#include "Registry.h"

namespace ECS
{
	Registry::Registry()
		: Entities{}
		, CurrentEntityCounter{}
		, ComponentPools{}
	{}

	Registry::~Registry()
	{
		for (int i{ static_cast<int>(Entities.Size()) - 1 }; i >= 0; --i)
		{
			ReleaseEntity(Entities[i]);
		}
	}

	Registry::Registry(Registry&& other) noexcept
		: Entities{ std::move(other.Entities) }
		, CurrentEntityCounter{ std::move(other.CurrentEntityCounter) }
		, ComponentPools{ std::move(other.ComponentPools) }
	{
		other.Entities.Clear();
		other.CurrentEntityCounter = 0;
		other.ComponentPools.clear();
	}

	Registry& Registry::operator=(Registry&& other) noexcept
	{
		Entities = std::move(other.Entities);
		CurrentEntityCounter = std::move(other.CurrentEntityCounter);
		ComponentPools = std::move(other.ComponentPools);

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

		return entity;
	}

	bool Registry::ReleaseEntity(Entity entity)
	{
		if (Entities.Contains(entity))
		{
			RemoveAllComponents(entity);

			Entities.Remove(entity);

			--CurrentEntityCounter;

			return true;
		}

		return false;
	}

	void Registry::RemoveAllComponents(const Entity entity)
	{
		for (std::pair<const std::string, std::unique_ptr<IComponentStorage>>& pair : ComponentPools)
		{
			if (pair.second->HasEntity(entity))
			{
				pair.second->Remove(entity);
			}
		}
	}
}