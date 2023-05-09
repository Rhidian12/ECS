#pragma once

#include <typeinfo> 
#include <vector>

namespace GO
{
	// Naive and simple implementation of GameObject and Components
	class GameObject final
	{
	public:
		~GameObject();

		void AddComponent(class Component* pComponent);

		template<typename T>
		T* GetComponent() const
		{
			const std::type_info& typeInfo{ typeid(T) };

			for (Component* pC : Components)
				if (typeid(*pC) == typeInfo)
					return static_cast<T*>(pC);

			return nullptr;
		}

		void Update();

	private:
		std::vector<class Component*> Components;
	};
}