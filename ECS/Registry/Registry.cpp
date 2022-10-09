#include "Registry.h"

namespace ECS
{
	Registry::Registry(const size_t nrOfEntitiesPerList)
		: EntitySignatures{}
		, Entities{}
		, CurrentEntityCounter{}
		, NrOfEntitiesPerList{ nrOfEntitiesPerList }
	{}

	Registry::~Registry()
	{
		for (std::vector<Entity>& list : Entities)
		{
			for (int i{ static_cast<int>(list.size()) - 1 }; i >= 0; --i)
			{
				ReleaseEntity(list[i]);
			}
		}
	}

	Registry::Registry(Registry&& other) noexcept
		: EntitySignatures{ std::move(other.EntitySignatures) }
		, ComponentPools{ std::move(other.ComponentPools) }
		, Entities{ std::move(other.Entities) }
		, CurrentEntityCounter{ std::move(other.CurrentEntityCounter) }
		, NrOfEntitiesPerList{ std::move(other.NrOfEntitiesPerList) }
	{
		other.EntitySignatures.clear();
		other.Entities.clear();
		other.CurrentEntityCounter = 0;
		other.ComponentPools.clear();
	}

	Registry& Registry::operator=(Registry&& other) noexcept
	{
		EntitySignatures = std::move(other.EntitySignatures);
		Entities = std::move(other.Entities);
		CurrentEntityCounter = std::move(other.CurrentEntityCounter);
		ComponentPools = std::move(other.ComponentPools);
		NrOfEntitiesPerList = std::move(other.NrOfEntitiesPerList);

		other.EntitySignatures.clear();
		other.Entities.clear();
		other.CurrentEntityCounter = 0;
		other.ComponentPools.clear();

		return *this;
	}

	Entity Registry::CreateEntity()
	{
		assert(CurrentEntityCounter <= MaxEntities);

		const Entity entity(CurrentEntityCounter++);

		if (entity % NrOfEntitiesPerList == 0)
		{
			Entities.push_back(std::vector<Entity>{});
		}

		Entities[entity / NrOfEntitiesPerList].push_back(entity);

		EntitySignatures.insert(std::make_pair(entity, EntitySignature{}));

		return entity;
	}

	bool Registry::ReleaseEntity(Entity entity)
	{
		if (HasEntity(entity))
		{
			RemoveAllComponents(entity, GetEntitySignature(entity));

			EntitySignatures.erase(entity);

			Entities[entity / NrOfEntitiesPerList].erase(
				std::remove(
					Entities[entity / NrOfEntitiesPerList].begin(),
					Entities[entity / NrOfEntitiesPerList].end(),
					entity),
				Entities[entity / NrOfEntitiesPerList].end());

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

				ComponentPools[i]->Remove(entity);
			}
		}
	}

	bool Registry::HasEntity(const Entity entity) const
	{
		return std::find(Entities[entity / NrOfEntitiesPerList].cbegin(),
			Entities[entity / NrOfEntitiesPerList].cend(),
			entity) != Entities[entity / NrOfEntitiesPerList].cend();
	}
}