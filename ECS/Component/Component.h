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
	concept IsDerivedComponent = std::is_base_of_v<Type, class Component>;

	template<IsDerivedComponent DerivedComponent>
	class Component : public IComponent
	{
	public:
		virtual ~Component() = default;

		__forceinline auto GetComponentID() const noexcept { return ComponentID; }

	protected:
		const ComponentType ComponentID{ FamilyTypeID<IComponent>::Get<DerivedComponent>() };
	};
}