#include "System.h"

namespace ECS
{
	Entity System::CreateEntity() const
	{
		return EntityManager::GetInstance()->CreateEntity();
	}
}