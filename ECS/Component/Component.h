#pragma once
#include "../ECSConstants.h"
#include "../TypeCounter/TypeCounter.h"

namespace ECS
{
	template<typename DerivedComponent>
	class IComponent
	{
	public:
		virtual ~IComponent() = default;

		static ComponentType GetComponentID() { return ComponentID; }

	protected:
		inline static constexpr ComponentType ComponentID{ GenerateID<DerivedComponent>() };
	};
}