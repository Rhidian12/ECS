#include "ComponentManager.h"

namespace ECS
{
	ComponentManager* ComponentManager::GetInstance()
	{
		if (!Instance)
		{
			Instance = std::make_unique<ComponentManager>();
		}

		return Instance.get();
	}
}