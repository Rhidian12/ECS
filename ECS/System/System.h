#pragma once
#include "../ECSConstants.h"
#include "../TypeCounter/TypeCounter.h"

#include "../Component/Component.h"

#include <array> /* std::array */
#include <vector> /* std::vector */
#include <assert.h> /* assert() */
#include <utility> /* std::move(), ... */
#include <functional> /* std::function */


#include <iostream>

namespace ECS
{
	template<typename ... Components>
	class View final
	{
	public:
		View(std::vector<std::tuple<Components&...>>&& components)
			: EntityComponents{ std::move(components) }
			, ComponentsSize{ components.size() }
		{}

		void ForEach(const std::function<void(Components&...)>& function) noexcept
		{
			for (size_t i{}; i < ComponentsSize; ++i)
			{
				std::apply(function, EntityComponents[i]);
			}
		}

	private:
		std::vector<std::tuple<Components&...>> EntityComponents;
		size_t ComponentsSize;
	};

	class System final
	{
	public:
		~System();

		Entity CreateEntity() noexcept;

		template<typename Component>
		void AddComponent(const Entity& entity) noexcept;

		template<typename ... TComponents>
		View<TComponents&...> CreateView() const noexcept
		{
			std::bitset<MaxComponentTypes> componentFlags{};
			SetComponentFlags<TComponents...>(componentFlags);

			return CreateView<TComponents&...>(componentFlags, std::make_index_sequence<sizeof ... (TComponents)>{});
		}

#ifdef DEBUG
		template<typename Component>
		Component* const GetComponent(const Entity& id) const noexcept;
#else
		template<typename Component>
		__forceinline Component* const GetComponent(const Entity& id) const noexcept { return static_cast<Component*>(Components[Component::GetComponentID()].Components[id]); }
#endif

		template<typename ... TComponents>
		__forceinline std::array<ComponentType, sizeof...(TComponents)> GetComponents() const noexcept { return { (GetComponentInfo<TComponents>(), ...) }; }
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

		template<typename ... TComponents>
		__forceinline void SetComponentFlags(EntitySignature& flags) const noexcept { (flags.set(TComponents::GetComponentID()), ...); }

		template<typename ... TComponents, size_t ... Indices>
		View<TComponents&...> CreateView(const EntitySignature& flags, std::index_sequence<Indices...>) const noexcept
		{
			std::vector<std::tuple<TComponents&...>> components{};
			const size_t componentsSize{ Entities.size() };

			// ((Indices < componentsSize ? components.push_back(CreateTuple<Indices, TComponents&...>()) : void()), ...);
			// return View<TComponents&...>{ components };

			for (size_t i{}; i < componentsSize; ++i)
			{
				components.push_back(CreateTuple<TComponents&...>(std::move(i)));
			}

			return View<TComponents&...>{ std::move(components) };
		}

		template<size_t Index, typename ... TComponents>
		std::tuple<TComponents& ...> CreateTuple() const noexcept
		{
			return std::tuple<TComponents&...>{ static_cast<TComponents&>(*Components[std::remove_reference_t<TComponents>::GetComponentID()].Components[Index])... };
		}

		template<typename ... TComponents>
		std::tuple<TComponents& ...> CreateTuple(size_t&& index) const noexcept
		{
			return std::tuple<TComponents&...>{ static_cast<TComponents&>(*Components[std::remove_reference_t<TComponents>::GetComponentID()].Components[index])... };
		}

		struct ComponentInfo final
		{
			ComponentType ComponentID = InvalidComponentID;
			std::vector<IComponent*> Components;
		};

		std::vector<EntitySignature> EntitySignatures;
		std::vector<Entity> Entities;
		std::vector<ComponentInfo> Components;
	};

	template<typename Component>
	void System::AddComponent(const Entity& entity) noexcept
	{
		assert(entity != InvalidEntityID);

		const ComponentType componentID{ Component::GetComponentID() };

		if (static_cast<size_t>(componentID) >= Components.size())
		{
			Components.resize(Components.size() + (componentID - Components.size() + 1));
		}

		ComponentInfo& key{ Components[componentID] };

		if (key.ComponentID == InvalidComponentID)
		{
			Components[componentID].ComponentID = componentID;
		}

		if (static_cast<size_t>(entity) >= key.Components.size())
		{
			key.Components.resize(key.Components.size() + (entity == 0 ? entity + 1 : entity));
		}

		key.Components[entity] = new Component{};

		EntitySignatures[entity].set(componentID);
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