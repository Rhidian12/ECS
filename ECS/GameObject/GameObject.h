#pragma once

#include <vector> /* std::vector */

namespace GO
{
	class GameObject final
	{
	public:
		~GameObject();

		void AddComponent(class Component* pComponent);

		void Update();

	private:
		std::vector<class Component*> Components;
	};
}