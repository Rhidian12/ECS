#pragma once
#include "../ECSConstants.h"
#include "../TypeCounter/TypeCounter.h"

#include "../Component/Component.h"

#include <array> /* std::array */
#include <vector> /* std::vector */
#include <assert.h>

namespace ECS
{
	class System final
	{
	public:
		~System();

#ifdef DEBUG
		Entity CreateEntity();
#else
		__forceinline Entity CreateEntity() { return Entities.emplace_back(static_cast<Entity>(Entities.size())); }
#endif

		template<typename Component>
		void AddComponent(const Entity& entity);

#ifdef DEBUG
		template<typename Component>
		Component* const GetComponent(const Entity& id) const noexcept;
#else
		template<typename Component>
		__forceinline Component* const GetComponent(const Entity& id) const noexcept { return static_cast<Component*>(Components[Component::GetComponentID()].Components[id]); }
#endif

		template<typename ... Components>
		__forceinline std::array<ComponentType, sizeof...(Components)> GetComponents() const noexcept { return { (GetComponentInfo<Components>(), ...) }; }
		__forceinline const auto& GetEntities() const noexcept { return Entities; }
		__forceinline auto& GetEntities() noexcept { return Entities; }

	private:
#ifdef DEBUG
		template<typename Component>
		ComponentType GetComponentInfo() const noexcept;
#else
		template<typename Component>
		__forceinline ComponentType GetComponentInfo() const noexcept { Components[Component::GetComponentID()].ComponentID; }
#endif

		struct ComponentInfo final
		{
			ComponentType ComponentID = InvalidComponentID;
			std::vector<IComponent*> Components;
		};

		std::vector<Entity> Entities;
		std::vector<ComponentInfo> Components;
	};
	
	template<typename Component>
	void System::AddComponent(const Entity& entity)
	{
		assert(entity != InvalidEntityID);

		const ComponentType componentID{ Component::GetComponentID() };

		if (static_cast<size_t>(componentID) >= Components.size())
		{
			Components.resize(Components.size() + componentID + 1);
		}

		ComponentInfo& key{ Components[componentID] };

		if (key.ComponentID == InvalidComponentID)
		{
			Components[componentID].ComponentID = componentID;
		}

		if (static_cast<size_t>(entity) >= key.Components.size())
		{
			key.Components.resize(key.Components.size() + entity + 1);
		}

		key.Components[entity] = new Component{};
	}
	
#ifdef DEBUG
	template<typename Component>
	Component* const System::GetComponent(const Entity& id) const noexcept
	{
		assert(id != InvalidEntityID);
		assert(Components[Component::GetComponentID()].ComponentID != InvalidComponentID);
		assert(static_cast<size_t>(id) <= Components[Component::GetComponentID()].Components.size());

		return static_cast<Component*>(Components[Component::GetComponentID()].Components[id]);
	}
	
	template<typename Component>
	ComponentType System::GetComponentInfo() const noexcept
	{
		assert(Components[Component::GetComponentID()].ComponentID != InvalidComponentID);

		return Components[Component::GetComponentID()].ComponentID;
	}
#endif
}