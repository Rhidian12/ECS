#include "SystemManager.h"

namespace ECS
{
	SystemManager::~SystemManager()
	{
		for (SystemInfo& info : Systems)
		{
			//delete info.pSystem;
			info.pSystem = nullptr;
		}
	}
}