#pragma once
#include "../ECSConstants.h"
#include "../ComponentTypeCounter/ComponentTypeCounter.h"

#include <concepts> /* Concepts */
#include <unordered_set> /* std::unordered_set */

namespace ECS
{
	class ISystem
	{
	public:
		virtual ~ISystem() = default;
	};

	template<typename Type>
	concept IsDerivedSystem = std::is_base_of_v<class System, Type>;

	template<IsDerivedSystem DerivedSystem>
	class System
	{
	public:
		virtual ~System() = default;

		void AddEntity(const Entity id) noexcept;
		void RemoveEntity(const Entity id) noexcept;

		virtual void UpdateSystem() = 0; /* This function will never be called from a System*, it just enforces inheritance */

		static __forceinline auto GetSystemID() noexcept { return SystemID; }

	protected:
		inline static const SystemID SystemID{ ComponentTypeCounter<ISystem>::Get<DerivedSystem>() };

		std::unordered_set<Entity> Entities;
	};

	template<IsDerivedSystem DerivedSystem>
	void System<DerivedSystem>::AddEntity(const Entity id) noexcept
	{
		Entities.insert(id);
	}

	template<IsDerivedSystem DerivedSystem>
	inline void System<DerivedSystem>::RemoveEntity(const Entity id) noexcept
	{
		Entities.erase(id);
	}
}