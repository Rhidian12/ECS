#include "Registry.h"

#include <algorithm>

namespace ECS
{
	Registry::Registry()
		: EntitySignatures{}
		, Entities{}
		, CurrentEntityCounter{}
	{}

	Registry::~Registry()
	{
		for (Entity& entity : Entities)
			ReleaseEntity(entity);

		Entities.clear();
	}

	Registry::Registry(Registry&& other) noexcept
		: EntitySignatures{ std::move(other.EntitySignatures) }
		, ComponentPools{ std::move(other.ComponentPools) }
		, Entities{ std::move(other.Entities) }
		, CurrentEntityCounter{ std::move(other.CurrentEntityCounter) }
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

		other.EntitySignatures.clear();
		other.Entities.clear();
		other.CurrentEntityCounter = 0;
		other.ComponentPools.clear();

		return *this;
	}

	Entity Registry::CreateEntity()
	{
		assert(CurrentEntityCounter <= MaxEntities);

		const Entity entity{ CurrentEntityCounter++ }; // [TODO]: We should recycle used entities

		Entities.push_back(entity);

		EntitySignatures.push_back(std::make_pair(entity, EntitySignature{}));

		return entity;
	}

	bool Registry::ReleaseEntity(Entity& entity)
	{
		if (HasEntity(entity))
		{
			RemoveAllComponents(entity, GetEntitySignature(entity)); // [TODO]: Come up with a better way to handle this

			EntitySignatures[entity].first = InvalidEntityID;
			EntitySignatures[entity].second = EntitySignature{};

			entity = InvalidEntityID;

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
				assert(GetComponentArray(i));

				GetComponentArray(i)->Remove(entity);
			}
		}
	}

	bool Registry::HasEntity(const Entity entity) const
	{
		return (entity < Entities.size() && Entities[entity] != InvalidEntityID);
	}

	void Registry::SetEntitySignature(const Entity entity, const ComponentType id, const bool val)
	{
		assert(HasEntity(entity));
		EntitySignatures[entity].second.set(id, val);
	}

	const EntitySignature& Registry::GetEntitySignature(const Entity entity) const
	{
		assert(HasEntity(entity));
		return EntitySignatures[entity].second;
	}

	std::unique_ptr<IComponentArray>& Registry::GetComponentArray(const ComponentType cType)
	{
		const auto cIt{ std::find_if(ComponentPools.begin(), ComponentPools.end(), [cType](const auto& kvPair) -> bool
			{
				return kvPair.first == cType;
			}) };

		if (cIt != ComponentPools.end())
		{
			return cIt->second;
		}
		else
		{
			return ComponentPools.emplace_back(std::make_pair(cType, nullptr)).second;
		}
	}
}