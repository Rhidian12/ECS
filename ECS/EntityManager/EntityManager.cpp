#include "EntityManager.h"

#include <assert.h> /* assert() */

namespace ECS
{
	EntityManager::EntityManager()
	{
		for (Entity id{}; id < MaxEntities; ++id)
		{
			AvailableEntityIDs.push(id);
		}
	}

	Entity EntityManager::CreateEntity() noexcept
	{
		assert(AvailableEntityIDs.size() > 0);

		Entity id{ AvailableEntityIDs.front() };
		AvailableEntityIDs.pop();

		return id;
	}

	void EntityManager::DestroyEntity(const Entity id) noexcept
	{
		assert(id < MaxEntities);

		/* Reset the bitfield (aka no more components added to this entity) */
		EntitySignatures[id].reset();

		/* Make the entity available to be used */
		AvailableEntityIDs.push(id);
	}

	void EntityManager::SetSignatureSafe(const Entity id, const ComponentType& signature) noexcept
	{
		assert(id < MaxEntities);

		EntitySignatures[id].set(signature);
	}

	const EntitySignature& EntityManager::GetEntitySignatureSafe(const Entity id) noexcept
	{
		assert(id < MaxEntities);

		return EntitySignatures[id];
	}
}