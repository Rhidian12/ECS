#pragma once

#include <vector> /* std::vector */
#include <typeinfo>

namespace GO
{
	class GameObject final
	{
	public:
		~GameObject();

		void AddComponent(class Component* pComponent);

		template<typename Type>
		Type* GetComponent() const
		{
			const std::type_info& typeInfo{ typeid(Type) };

			for (Component* pC : Components)
				if (typeid(*pC) == typeInfo)
					return static_cast<Type*>(pC);

			return nullptr;
		}

		void Update();

	private:
		std::vector<class Component*> Components;
	};
}