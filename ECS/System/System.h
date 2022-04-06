#pragma once
#include "../ECSConstants.h"
#include "../TypeCounter/TypeCounter.h"

#include "../Component/Component.h"

#include <array> /* std::array */
#include <vector> /* std::vector */
#include <assert.h> /* assert() */
#include <utility> /* std::move(), ... */
#include <functional> /* std::function */

namespace ECS
{
	template<typename ... Components>
	class View final
	{
	public:
		View(const std::vector<std::tuple<Components*...>>& components)
			: Components{ components }
			, ComponentsSize{ components.size() }
		{}

		void ForEach(std::function<void(Components&...)> function) const noexcept
		{
			ForEach(std::move(function), std::make_index_sequence<MaxEntities>{});
		}

	private:
		template<size_t ... Indices>
		void ForEach(std::function<void(Components&...)>&& function, std::index_sequence<Indices...>) const noexcept
		{
			((Indices < ComponentsSize ? ApplyFunction<Indices>(std::move(function), std::make_index_sequence<sizeof ... (Components)>{}) : void()), ...);
		}

		template<size_t Index, size_t ... Indices>
		void ApplyFunction(std::function<void(Components&...)>&& function, std::index_sequence<Indices...>) const noexcept
		{
			function((std::get<Indices>(Components[Index]), ...));
		}

		std::vector<std::tuple<Components*...>> Components;
		size_t ComponentsSize;
	};

	class System final
	{
	public:
		~System();

		Entity CreateEntity() noexcept;

		template<typename Component>
		void AddComponent(const Entity& entity) noexcept;

		template<typename ... Components>
		View<Components...> CreateView() const noexcept
		{
			std::vector<std::tuple<Components*...>> components{};

			std::bitset<MaxComponentTypes> componentFlags{};
			SetComponentFlags<Components...>(componentFlags);

			return CreateView<Components...>(componentFlags, std::make_index_sequence<MaxEntities>{});
		}

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

		template<typename ... Components>
		__forceinline void SetComponentFlags(EntitySignature& flags) const noexcept { (flags.set(Components::GetComponentID()), ...); }

		template<typename ... Components, size_t ... Indices>
		View<Components...> CreateView(const EntitySignature& flags, std::index_sequence<Indices...>) const noexcept
		{
			std::vector<std::tuple<Components*...>> components{};
			const size_t componentsSize{ this->Components.size() };

			( (Indices < componentsSize ? components.push_back(CreateTuple<Indices, Components...>(std::make_index_sequence<sizeof ... (Components)>)) : void()), ... );
			return View<Components...>{ components };
		}

		template<size_t Index, typename ... Components, size_t ... Indices>
		std::tuple<Components* ...> CreateTuple(std::index_sequence<Indices...>) const noexcept
		{
			std::make_tuple( (this->Components[Index].Components[Indices], ...) );
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