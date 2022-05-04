#pragma once
#include "../ECSConstants.h"

namespace ECS
{
	/* This class is heavily based on a class made by the absolute genius Arne Van Kerschaver */
	template <typename Type>
	class ComponentCounter final
	{
	public:
		/* Since it's ++Counter, the Component Counter will always start at 1, can be easily changed, but who cares */
		template <typename SecondaryType>
		__forceinline constexpr static ComponentType Get() { return Counter++; }

		__forceinline constexpr static ComponentType Get() { return Counter; }

	private:
		inline static ComponentType Counter{};
	};
}