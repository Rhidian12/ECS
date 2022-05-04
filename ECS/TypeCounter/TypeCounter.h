#pragma once
#include "../ECSConstants.h"

namespace ECS
{
	/* This class is heavily based on a class made by the absolute genius Arne Van Kerschaver */
	template <typename Type>
	class ComponentCounter final
	{
	public:
		template <typename SecondaryType>
		constexpr static ComponentType Get() { return Counter++; }

		constexpr static ComponentType Get() { return Counter; }

	private:
		inline static ComponentType Counter{};
	};
}