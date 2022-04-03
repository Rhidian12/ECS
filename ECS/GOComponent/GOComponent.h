#pragma once

namespace GO
{
	class Component
	{
	public:
		virtual ~Component() = default;

		virtual void Update() {}
	};
}

