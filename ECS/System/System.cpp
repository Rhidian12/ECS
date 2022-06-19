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

	void System::ClearEntities()
	{
		Entities.Clear();
	}

	void System::ReleaseEntities()
	{
		for (const Entity entity : Entities)
		{
			if (EntityManager::GetInstance()->ReleaseEntity(entity))
			{
			}
		}

		Entities.Clear();
	}
}