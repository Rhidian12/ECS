#include "Registry.h"

#include <algorithm>
#include <assert.h>

namespace ECS
{
	Registry::Registry()
		: Entities{}
		, CurrentEntityCounter{}
	{}

	Registry::~Registry()
	{
		Clear();
	}

	Registry::Registry(Registry&& other) noexcept
		: ComponentPools{ std::move(other.ComponentPools) }
		, Entities{ std::move(other.Entities) }
		, CurrentEntityCounter{ std::move(other.CurrentEntityCounter) }
		, RecycledEntities{ std::move(other.RecycledEntities) }
	{
		other.Entities.Clear();
		other.CurrentEntityCounter = 0;
		other.ComponentPools.clear();

		while (!other.RecycledEntities.empty()) other.RecycledEntities.pop();
	}

	Registry& Registry::operator=(Registry&& other) noexcept
	{
		Entities = std::move(other.Entities);
		CurrentEntityCounter = std::move(other.CurrentEntityCounter);
		ComponentPools = std::move(other.ComponentPools);
		RecycledEntities = std::move(other.RecycledEntities);

		other.Entities.Clear();
		other.CurrentEntityCounter = 0;
		other.ComponentPools.clear();

		while (!other.RecycledEntities.empty()) other.RecycledEntities.pop();

		return *this;
	}

	Entity Registry::CreateEntity()
	{
		assert(CurrentEntityCounter <= MaxEntities);

		Entity entity{};

		if (!RecycledEntities.empty())
		{
			entity = RecycledEntities.front();
			RecycledEntities.pop();
		}
		else
		{
			entity = CurrentEntityCounter++;
		}

		Entities.Add(entity);

		return entity;
	}

	bool Registry::ReleaseEntity(const Entity entity)
	{
		if (HasEntity(entity))
		{
			RemoveAllComponents(entity); // [TODO]: Come up with a better way to handle this

			Entities.Remove(entity);

			RecycledEntities.push(entity);

			return true;
		}

		return false;
	}

	void Registry::RemoveAllComponents(const Entity entity)
	{
		for (const auto& compArray : ComponentPools)
		{
			compArray.second->Remove(entity);
		}
	}

	bool Registry::HasEntity(const Entity entity) const
	{
		return Entities.Contains(entity);
	}

	void Registry::Clear()
	{
		Entities.Clear();
		CurrentEntityCounter = 0;

		for (const auto& [component, compArray] : ComponentPools)
		{
			compArray->RemoveAll();
		}

		ComponentPools.clear();
		
		while (!RecycledEntities.empty()) RecycledEntities.pop();
	}

	const std::unique_ptr<IComponentArray>& Registry::GetComponentArray(const size_t cType) const
	{
		const auto cIt{ std::find_if(ComponentPools.cbegin(), ComponentPools.cend(), [cType](const auto& kvPair)->bool
			{
				return kvPair.first == cType;
			}) };

		assert(cIt != ComponentPools.end());

		return cIt->second;
	}

	std::unique_ptr<IComponentArray>& Registry::GetComponentArray(const size_t cType)
	{
		const auto it{ std::find_if(ComponentPools.begin(), ComponentPools.end(), [cType](const auto& kvPair)->bool
			{
				return kvPair.first == cType;
			}) };

		if (it != ComponentPools.end())
		{
			return it->second;
		}
		else
		{
			return ComponentPools.emplace_back(std::make_pair(cType, nullptr)).second;
		}
	}
}