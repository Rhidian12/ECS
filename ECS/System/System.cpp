#include "System.h"

namespace ECS
{
	System::~System()
	{
		ReleaseEntities();
	}

	void System::AddEntity(Entity entity)
	{
		Entities.Add(entity);
	}

	Entity System::CreateEntity()
	{
		const Entity entity(EntityManager::GetInstance()->CreateEntity());
		Entities.Add(entity);

		return entity;
	}

	void System::RemoveEntitiy(const Entity entity)
	{
		assert(Entities.Contains(entity));

		Entities.Remove(entity);
	}

	void System::ClearEntities()
	{
		Entities.Clear();
	}

	void System::ReleaseEntities()
	{
		for (const Entity entity : Entities)
		{
			EntityManager::GetInstance()->ReleaseEntity(entity);
		}

		Entities.Clear();
	}
}