#pragma once
#include "../ECSConstants.h"
#include "../System/System.h"

#include <array> /* std::array */

namespace ECS
{
	class SystemManager final
	{
	public:
		~SystemManager();

		template<typename DerivedSystem>
		DerivedSystem* const AddSystem() noexcept;

		template<typename DerivedSystem>
		DerivedSystem* const GetSystem() noexcept;

		template<typename ... Systems>
		void Update() noexcept;

	private:
		struct SystemInfo final
		{
			SystemInfo()
				: SystemID{ InvalidSystemID }
				, pSystem{}
			{}

			SystemInfo(const SystemID& id, class ISystem* _pSystem)
				: SystemID{ id }
				, pSystem{ _pSystem }
			{}

			SystemID SystemID;
			ISystem* pSystem;
		};

		std::array<SystemInfo, MaxSystems> Systems;

		template<typename ... Systems, size_t ... Indices>
		void Update(std::index_sequence<Indices...>);

		template<typename System, size_t Index>
		void Update();
	};

	template<typename DerivedSystem>
	DerivedSystem* const SystemManager::AddSystem() noexcept
	{
		const SystemID systemID{ DerivedSystem::GetSystemID() };
		SystemInfo& systemInfo{ Systems[systemID] };

		if (systemInfo.SystemID == InvalidSystemID)
		{
			systemInfo.SystemID = systemID;
		}

		/* Safety check to avoid memory leaks */
		if (systemInfo.pSystem)
		{
			delete systemInfo.pSystem;
			systemInfo.pSystem = nullptr;
		}

		systemInfo.pSystem = new DerivedSystem{};

		return static_cast<DerivedSystem*>(systemInfo.pSystem);
	}

	template<typename DerivedSystem>
	DerivedSystem* const SystemManager::GetSystem() noexcept
	{
		const SystemID systemID{ DerivedSystem::GetSystemID() };

		if (systemID == InvalidSystemID)
			return nullptr;

		const SystemInfo& systemInfo{ Systems[systemID] };

		if (systemInfo.SystemID != InvalidSystemID)
		{
			return static_cast<DerivedSystem*>(systemInfo.pSystem);
		}

		return nullptr;
	}
	
	template<typename ...Systems>
	void SystemManager::Update() noexcept
	{
		Update<Systems...>(std::make_index_sequence<sizeof...(Systems)>{});
	}
	
	template<typename ...Systems, size_t ...Indices>
	void SystemManager::Update(std::index_sequence<Indices...>)
	{
		(Update<Systems, Indices>(), ...);
	}

	template<typename System, size_t Index>
	void SystemManager::Update()
	{
		static_cast<System*>(Systems[Index].pSystem)->UpdateSystem();
	}
}