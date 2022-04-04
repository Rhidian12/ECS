#pragma once
#include "../ECSConstants.h"
#include "../System/System.h"

#include "../TypeList/TypeList.h"

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

		void Update() noexcept;

	private:
		struct SystemInfo final
		{
			SystemInfo()
				: SystemID{ InvalidSystemID }
				, pSystem{}
			{}

			SystemInfo(const SystemID& id, ISystem* _pSystem)
				: SystemID{ id }
				, pSystem{ _pSystem }
			{}

			SystemID SystemID;
			ISystem* pSystem;
		};

		std::array<SystemInfo, MaxSystems> Systems;

		using SystemTypes = typelist::tlist<>;

		template<size_t Index>
		void ExecuteSystems();

		template<size_t ... Indices>
		void ExecuteSystems(std::index_sequence<Indices...>);
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
		using SystemTypes = typelist::tlist_push_back<DerivedSystem, SystemTypes>::type;

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

	template<size_t Index>
	void SystemManager::ExecuteSystems()
	{
		static_cast<typelist::tlist_type_at<Index, SystemTypes>::type*>(Systems[Index].pSystem)->UpdateSystem();
	}

	template<size_t ...Indices>
	void SystemManager::ExecuteSystems(std::index_sequence<Indices...>)
	{
		(ExecuteSystems<Indices>(), ...);
	}
}