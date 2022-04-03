#pragma once
#include "../ECSConstants.h"
#include "../Component/Component.h"

#include <vector> /* std::vector */
#include <algorithm> /* std::find, std::find_if, ... */

namespace ECS
{
	class ComponentManager final
	{
	public:
		template<typename Type>
		void AddComponent(const Entity id, IComponent* const pComponent) noexcept;

		template<typename Type>	
		Type* GetComponent(const Entity id, const ComponentType ComponentID) const noexcept;

	private:
		struct ComponentData
		{
			Entity EntityID;
			IComponent* pComponent;
		};
		struct ComponentKey final
		{
			ComponentType ComponentID;
			std::vector<ComponentData> ComponentsData;
		};

		/* http://scottmeyers.blogspot.com/2015/09/should-you-be-using-something-instead.html */
		/* Why I use a vector over a std::unordered_map */
		std::vector<ComponentKey> Components;

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
		Component<Type>* const pCastComponent{ static_cast<Component<Type>*>(pComponent) };
		const ComponentType componentID{ pCastComponent->GetComponentID() };

		auto it{ std::find_if(Components.begin(), Components.end(), [&componentID](const ComponentKey& key)->bool
			{
				return key.ComponentID == componentID;
			})};

		/* This type of component has not been added yet, so add it */
		if (it == Components.end())
		{
			Components.push_back(ComponentKey{ componentID, std::vector<ComponentData>{ { id, pComponent } } });
		}
		else /* This type of component has already been added, so add it to the vector */
		{
			it->ComponentsData.push_back({ id, pComponent });
		}
	}

	template<typename Type>
	Type* ComponentManager::GetComponent(const Entity id, const ComponentType componentID) const noexcept
	{
		auto it = std::find_if(Components.begin(), Components.end(), [&componentID](const ComponentKey& key)->bool
			{
				return key.ComponentID == componentID;
			});

		if (it != Components.end())
		{
			auto componentKeyCIt = std::find_if(it->ComponentsData.cbegin(), it->ComponentsData.cend(),[&id](const ComponentData& cData)->bool
			{
				return id == cData.EntityID;
			});

			if (componentKeyCIt != it->ComponentsData.cend())
			{
				return static_cast<Type*>(componentKeyCIt->pComponent);
			}
		}

		return nullptr;
	}
}