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
}