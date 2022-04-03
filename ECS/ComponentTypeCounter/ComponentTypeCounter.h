#pragma once
#include "../ECSConstants.h"

namespace ECS
{
	/* This class was made by the absolute genius Arne Van Kerschaver */
	template <class T>
	class ComponentTypeCounter
	{
	public:
		/* Since it's ++Counter, the Component Counter will always start at 1, can be easily changed, but who cares */
		template <typename U>
		__forceinline constexpr static ComponentType Get() { return ++Counter; }

		__forceinline constexpr static ComponentType Get() { return Counter; }

	private:
		inline static ComponentType Counter{};
	};
}