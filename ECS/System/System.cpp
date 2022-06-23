#include "System.h"

namespace ECS
{
	System::~System()
	{
		ReleaseEntities();
	}

	void System::AddEntity(Entity entity)
	{
		Entities.push_back(entity);
	}

	Entity System::CreateEntity()
	{
		const Entity entity(EntityManager::GetInstance()->CreateEntity());
		Entities.push_back(entity);

		return entity;
	}

	void System::ClearEntities()
	{
		Entities.clear();
	}

	void System::ReleaseEntities()
	{
		for (const Entity entity : Entities)
		{
			EntityManager::GetInstance()->ReleaseEntity(entity);
		}

		Entities.clear();
	}
}