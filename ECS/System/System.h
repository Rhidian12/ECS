#pragma once
#include "../ECSConstants.h"
#include "../TypeCounter/TypeCounter.h"

#include "../Component/Component.h"

#include <array> /* std::array */
#include <vector> /* std::vector */
#include <assert.h>

namespace ECS
{
	//class ISystem
	//{
	//public:
	//	virtual ~ISystem() = default;
	//};

	//template<typename DerivedSystem>
	//class System : public ISystem
	//{
	//public:
	//	System();
	//	virtual ~System() = default;

	//	void AddEntity(const Entity id) noexcept;
	//	void RemoveEntity(const Entity id) noexcept;

	//	static __forceinline auto GetSystemID() noexcept { return SystemID; }

	//protected:
	//	inline static const SystemID SystemID{ TypeCounter<ISystem>::Get<DerivedSystem>() };

	//	std::array<Entity, MaxEntities> Entities;
	//};

	//template<typename DerivedSystem>
	//System<DerivedSystem>::System()
	//	: Entities{}
	//{
	//	for (Entity& entity : Entities)
	//	{
	//		entity = InvalidEntityID;
	//	}
	//}

	//template<typename DerivedSystem>
	//void System<DerivedSystem>::AddEntity(const Entity id) noexcept
	//{
	//	Entities[id] = id;
	//}

	//template<typename DerivedSystem>
	//inline void System<DerivedSystem>::RemoveEntity(const Entity id) noexcept
	//{
	//	Entities[id] = InvalidEntityID;
	//}

	class System final
	{
	public:
		~System()
		{
			for (ComponentInfo& info : Components)
			{
				for (IComponent*& pC : info.Components)
				{
					delete pC;
					pC = nullptr;
				}
			}
		}

		Entity CreateEntity()
		{
			return Entities.emplace_back(static_cast<Entity>(Entities.size()));
		}

		template<typename Component>
		void AddComponent(const Entity& entity)
		{
			assert(entity != InvalidEntityID);

			const ComponentType componentID{ Component::GetComponentID() };
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

		template<typename Component>
		Component* GetComponent(const Entity& id) const noexcept
		{
			assert(id != InvalidEntityID);
			assert(Components[Component::GetComponentID()].ComponentID != InvalidComponentID);
			assert(static_cast<size_t>(id) <= Components[Component::GetComponentID()].Components.size());

			return static_cast<Component*>(Components[Component::GetComponentID()].Components[id]);
		}

		template<typename ... Components>
		constexpr std::array<ComponentType, sizeof...(Components)> GetComponents() const noexcept
		{
			return { (GetComponentInfo<Components>(), ...) };
		}

		__forceinline const std::vector<Entity>& GetEntities() const noexcept { return Entities; }
		__forceinline std::vector<Entity>& GetEntities() noexcept { return Entities; }

	private:
		template<typename Component>
		__forceinline ComponentType GetComponentInfo() const noexcept
		{
			assert(Components[Component::GetComponentID()].ComponentID != InvalidComponentID);

			return Components[Component::GetComponentID()].ComponentID;
		}

		struct ComponentInfo final
		{
			ComponentType ComponentID = InvalidComponentID;
			std::vector<IComponent*> Components;
		};

		std::vector<Entity> Entities;
		std::array<ComponentInfo, MaxComponentTypes> Components;
	};
}