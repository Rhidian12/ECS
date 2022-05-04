#include "System.h"

namespace ECS
{
	Entity System::CreateEntity()
	{
		return EntityManager::GetInstance()->CreateEntity();
	}
}