#include "System.h"

namespace ECS
{
	void System::AddEntity(Entity entity)
	{
		Entities.Add(entity);
	}

	Entity System::CreateEntity()
	{
		Entities.Add(EntityManager::GetInstance()->CreateEntity());

		return Entities.Back();
	}

	void System::ClearEntities()
	{
		Entities.Clear();
	}

	void System::ReleaseEntities()
	{
		for (Entity entity{}; entity < Entities.Size(); ++entity)
		{
			EntityManager::GetInstance()->ReleaseEntity(entity);

			ComponentManager::GetInstance()->RemoveComponent(entity, EntityManager::GetInstance()->GetEntitySignature(entity));
		}

		Entities.Clear();
	}
}