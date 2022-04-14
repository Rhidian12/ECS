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
		View(std::tuple<std::vector<TComponents>...>&& components)
			: Components{ std::move(components) }
			, NrOfComponents{ std::get<0>(components).size() }
		{}

		void ForEach(const std::function<void(TComponents...)>& function) const
		{
			auto indexSequence{ std::make_index_sequence<sizeof ... (TComponents)>{} };
			
			for (int i{}; i < NrOfComponents; ++i)
			{
				ForEach(function, i, indexSequence);
			}
		}

	private:
		template<size_t ... Indices>
		void ForEach(const std::function<void(TComponents...)>& function, size_t index, std::index_sequence<Indices...>) const
		{
			std::tuple<TComponents...> comps{ std::make_tuple(std::get<Indices>(Components)[index]...)};
			std::apply(function, comps);
		}

		std::tuple<std::vector<TComponents>...> Components;
		size_t NrOfComponents;
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

			return CreateView<TComponents*...>(componentFlags, std::make_index_sequence<sizeof ... (TComponents)>{});
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
		View<TComponents...> CreateView(const EntitySignature& flags, std::index_sequence<Indices...>) const noexcept
		{
			return View<TComponents...>(CreateViewData<TComponents...>(std::make_index_sequence<sizeof ... (TComponents)>{}));
		}

		template<typename ... TComponents, size_t ... Indices>
		std::tuple<std::vector<TComponents>...> CreateViewData(std::index_sequence<Indices...>) const
		{
			return std::make_tuple(ConvertVectorContents<TComponents>(Components[Indices])...);
		}

		template<typename To, typename From>
		std::vector<To> ConvertVectorContents(const std::vector<From>& v) const
		{
			std::vector<To> vTo(v.size());

			if constexpr (std::is_convertible_v<From, To>)
			{
				for (From const pElement : v)
				{
					vTo.push_back(static_cast<To>(pElement));
				}
			}
			else
			{
				for (From const pElement : v)
				{
					vTo.push_back(dynamic_cast<To>(pElement));
				}
			}

			return vTo;
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

		components[entity] = new Component();

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