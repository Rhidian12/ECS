#pragma once
#include "../ECSConstants.h"
#include "../Component/Component.h"

#include <array> /* std::array */

namespace ECS
{
	class ComponentManager final
	{
	public:
		template<typename Type>
		void AddComponent(const Entity id, IComponent* const pComponent) noexcept;

		template<typename Type>
		Type* GetComponent(const Entity id) const noexcept;

	private:
		struct ComponentKey final
		{
			ComponentType ComponentID{ InvalidComponentID }; /* Make sure this is default init to an invalid ComponentType */
			std::array<IComponent*, MaxEntities> Components;
		};

		/* http://scottmeyers.blogspot.com/2015/09/should-you-be-using-something-instead.html */
		/* Why I use a vector over a std::unordered_map */
		std::array<ComponentKey, MaxComponentTypes> Components;

		/* The Component Manager needs to know about ALL types of components */
		/* The easy way is of course to just make a base Component and store a pointer to that */
		/* or make a ComponentArray that stores all Components and store a bunch of those */

		/* Option 1 is pretty decent, but will result in quite some virtual calls */
		/* Option 2 is just honestly supremely cringe cus it's even more inheritance */
		/* Disclaimer: Inheritance is amazing but it's avoidable here */

		/* There has to be a third option. I'm just struggling to come up with anything */

		/* Problem 1 (and the biggest problem): */
			/* How do I store Components? */
			/* I could store Component ID's as Keys, but I still need to store the Component data as well */

		/* At the moment I'll make a base class for Components and store pointers to those*/
	};


	template<typename Type>
	void ComponentManager::AddComponent(const Entity id, IComponent* const pComponent) noexcept
	{
		if (id == InvalidEntityID)
			return;

		const ComponentType componentID{ Type::GetComponentID() };
		ComponentKey& key{ Components[componentID] };

		if (key.ComponentID == InvalidComponentID)
		{
			Components[componentID].ComponentID = componentID;
		}

		key.Components[id] = pComponent;
	}

	template<typename Type>
	Type* ComponentManager::GetComponent(const Entity id) const noexcept
	{
		if (id == InvalidEntityID)
			return nullptr;

		const auto componentID{ Type::GetComponentID() };
		const ComponentKey& key{ Components[componentID] };

		if (key.ComponentID != InvalidComponentID)
		{
			return static_cast<Type*>(key.Components[id]);
		}
		else
		{
			return nullptr;
		}
	}
}