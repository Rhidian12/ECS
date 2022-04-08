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
	template<typename ... TComponents>
	class View final
	{
	public:
		View(std::array<std::tuple<TComponents...>, sizeof ... (TComponents)>&& components)
			: Components{ std::move(components) }
			, ComponentsSize{ components.size() }
		{}

		void ForEach(const std::function<void(TComponents...)>& function) noexcept
		{
			//for (size_t i{}; i < ComponentsSize; ++i)
			//{
			//	std::apply(function, Components[i]);
			//}

			return [this] <size_t ... Indices> (const std::function<void(TComponents...)>& function, std::index_sequence<Indices...>)
			{
				(std::apply(function, Components[Indices]), ...);
			} (function, std::make_index_sequence<sizeof ... (TComponents)>{});
		}

	private:
		std::array<std::tuple<TComponents...>, sizeof ... (TComponents)> Components;
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
		View<TComponents*...> CreateView() const noexcept
		{
			std::bitset<MaxComponentTypes> componentFlags{};
			SetComponentFlags<TComponents...>(componentFlags);

			return CreateView<TComponents...>(componentFlags, std::make_index_sequence<sizeof ... (TComponents)>{});
		}

#ifdef DEBUG
		template<typename Component>
		Component* const GetComponent(const Entity& id) const noexcept;
#else
		template<typename Component>
		__forceinline Component* const GetComponent(const Entity& id) const noexcept { return static_cast<Component*>(Components[Component::GetComponentID()][id]); }
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
		View<TComponents*...> CreateView(const EntitySignature& flags, std::index_sequence<Indices...>) const noexcept
		{
			// return std::array<std::vector<TComponents*...>, sizeof ... (TComponents)> { { (CreateTuple<TComponents...>(Indices), ...) } };
			return View<TComponents*...>{  };
			//const size_t componentsSize{ Entities.size() };

			// ((Indices < componentsSize ? components.push_back(CreateTuple<Indices, TComponents&...>()) : void()), ...);
			// return View<TComponents&...>{ components };

			// return View<TComponents*...>{ std::move(components) };
		}

		template<size_t Index, typename ... TComponents>
		std::tuple<TComponents& ...> CreateTuple() const noexcept
		{
			return std::tuple<TComponents&...>{ static_cast<TComponents&>(*Components[std::remove_reference_t<TComponents>::GetComponentID()][Index])... };
		}

		template<typename ... TComponents>
		std::tuple<TComponents*...> CreateTuple(size_t&& index) const noexcept
		{
			return std::make_tuple(static_cast<TComponents*>(Components[std::remove_reference_t<TComponents>::GetComponentID()][index])...);
		}

		template<typename ... TComponents, size_t ... Indices>
		std::array<std::vector<TComponents*...>, sizeof ... (TComponents)> CreateViewData(std::index_sequence<Indices...>) const noexcept
		{
			return std::array<std::vector<TComponents*...>, sizeof ... (TComponents)>{ std::vector<TComponents*>{ Components[Indices] } };
		}

		std::vector<EntitySignature> EntitySignatures;
		std::vector<Entity> Entities;
		std::vector<std::vector<IComponent*>> Components;
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

		std::vector<IComponent*>& components{ Components[componentID] };

		if (static_cast<size_t>(entity) >= components.size())
		{
			components.resize(components.size() + (entity - components.size() + 1));
		}

		components[entity] = new Component{};

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