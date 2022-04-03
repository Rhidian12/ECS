#pragma once
#include "../ECSConstants.h"
#include "../ComponentTypeCounter/ComponentTypeCounter.h"

#include <concepts> /* Concepts */

namespace ECS
{
	class IComponent
	{
	public:
		virtual ~IComponent() = default;
	};

	template<typename Type>
	concept IsDerivedComponent = std::is_base_of_v<class Component, Type>;

	template<IsDerivedComponent DerivedComponent>
	class Component : public IComponent
	{
	public:
		virtual ~Component() = default;

		static __forceinline auto GetComponentID() const noexcept { return ComponentID; }

	protected:
		inline static const ComponentType ComponentID{ FamilyTypeID<IComponent>::Get<DerivedComponent>() };
	};
}