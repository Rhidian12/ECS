#pragma once
#include "../ECSConstants.h"
#include "../TypeCounter/TypeCounter.h"

#include <array> /* std::array */

namespace ECS
{
	class ISystem
	{
	public:
		virtual ~ISystem() = default;
	};

	template<typename DerivedSystem>
	class System : public ISystem
	{
	public:
		System();
		virtual ~System() = default;

		void AddEntity(const Entity id) noexcept;
		void RemoveEntity(const Entity id) noexcept;

		static __forceinline auto GetSystemID() noexcept { return SystemID; }

	protected:
		inline static const SystemID SystemID{ TypeCounter<ISystem>::Get<DerivedSystem>() };

		std::array<Entity, MaxEntities> Entities;
	};

	template<typename DerivedSystem>
	System<DerivedSystem>::System()
		: Entities{}
	{
		for (Entity& entity : Entities)
		{
			entity = InvalidEntityID;
		}
	}

	template<typename DerivedSystem>
	void System<DerivedSystem>::AddEntity(const Entity id) noexcept
	{
		Entities[id] = id;
	}

	template<typename DerivedSystem>
	inline void System<DerivedSystem>::RemoveEntity(const Entity id) noexcept
	{
		Entities[id] = InvalidEntityID;
	}
}