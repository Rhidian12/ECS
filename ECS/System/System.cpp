#include "System.h"

namespace ECS
{
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