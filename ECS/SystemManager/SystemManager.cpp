#include "SystemManager.h"

namespace ECS
{
	SystemManager::~SystemManager()
	{
		for (SystemInfo& info : Systems)
		{
			delete info.pSystem;
			info.pSystem = nullptr;
		}

		Systems.clear();
	}

	void SystemManager::Update() noexcept
	{
		// constexpr size_t size{ SystemTypes::size() };
		ExecuteSystems(std::make_index_sequence<SystemTypes::size()>{});
	}
}