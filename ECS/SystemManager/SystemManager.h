#pragma once
#include "../ECSConstants.h"
#include "../System/System.h"

#include "../TypeList/TypeList.h"

#include <vector> /* std::vector */
#include <algorithm> /* std::find, std::find_if, ... */

namespace ECS
{
	class SystemManager final
	{
	public:
		~SystemManager();

		template<typename DerivedSystem>
		DerivedSystem* const AddSystem() noexcept;

		template<typename DerivedSystem>
		DerivedSystem* const GetSystem(const SystemID systemID) noexcept;

		void Update() noexcept;

	private:
		struct SystemInfo final
		{
			SystemID SystemID;
			ISystem* pSystem;
		};

		std::vector<SystemInfo> Systems;

		using SystemTypes = typelist::tlist<>;

		template<size_t Index>
		void ExecuteSystems();

		template<size_t ... Indices>
		void ExecuteSystems(std::index_sequence<Indices...>);
	};

	template<typename DerivedSystem>
	DerivedSystem* const SystemManager::AddSystem() noexcept
	{
		auto cIt{ std::find_if(Systems.cbegin(), Systems.cend(), [](const SystemInfo& sInfo)
			{
				return sInfo.SystemID == DerivedSystem::GetSystemID();
			}) };

		if (cIt == Systems.cend())
		{
			Systems.push_back(SystemInfo{ DerivedSystem::GetSystemID(), new DerivedSystem{} });

			SystemTypes = typelist::tlist_push_back<DerivedSystem, SystemTypes>::type;
		}
	}

	template<typename DerivedSystem>
	DerivedSystem* const SystemManager::GetSystem(const SystemID systemID) noexcept
	{
		auto cIt{ std::find_if(Systems.cbegin(), Systems.cend(), [&systemID](const SystemInfo& sInfo)
			{
				return sInfo.SystemID == systemID;
			}) };

		if (cIt != Systems.cend())
		{
			return static_cast<DerivedSystem*>(cIt->pSystem);
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