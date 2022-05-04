#include "EntityManager.h"

namespace ECS
{
	Entity EntityManager::CreateEntity()
	{
		assert(static_cast<Entity>(Entities.size()) < MaxEntities);

		EntitySignatures.push_back(EntitySignature{});
		Entities.push_back(static_cast<Entity>(Entities.size()));
		return Entities.back();
	}
}