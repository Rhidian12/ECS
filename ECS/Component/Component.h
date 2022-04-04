#pragma once
#include "../ECSConstants.h"
#include "../ComponentTypeCounter/ComponentTypeCounter.h"

namespace ECS
{
	class IComponent
	{
	public:
		virtual ~IComponent() = default;
	};

	template<typename DerivedComponent>
	class Component : public IComponent
	{
	public:
		virtual ~Component() = default;

		static __forceinline auto GetComponentID() noexcept { return ComponentID; }

	protected:
		inline static const ComponentType ComponentID{ ComponentTypeCounter<IComponent>::Get<DerivedComponent>() };
	};
}