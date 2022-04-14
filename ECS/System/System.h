#pragma once
#include "../ECSConstants.h"
#include "../TypeCounter/TypeCounter.h"

#include "../Component/Component.h"

#include <vector> /* std::vector */
#include <assert.h> /* assert() */
#include <utility> /* std::move(), ... */
#include <functional> /* std::function */

namespace ECS
{
	template<typename ... TComponents>
	class View final
	{
	public:
		View(std::tuple<std::vector<TComponents>...>&& components)
			: Components{ std::move(components) }
			, NrOfComponents{ std::get<0>(Components).size() }
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
			std::tuple<TComponents...> comps{ std::make_tuple(std::get<Indices>(Components)[index]...) };
			std::apply(function, comps);
		}

		std::tuple<std::vector<TComponents>...> Components;
		size_t NrOfComponents;
	};

	class System final
	{
	public:
		~System();

		Entity CreateEntity();

		template<typename Component>
		void AddComponent(const Entity& entity);

		template<typename ... TComponents>
		View<TComponents*...> CreateView() const
		{
			std::bitset<MaxComponentTypes> componentFlags{};
			SetComponentFlags<TComponents...>(componentFlags);

			return CreateView<TComponents*...>(componentFlags, std::make_index_sequence<sizeof ... (TComponents)>{});
		}

		template<typename Component>
		Component* const GetComponent(const Entity& id) const;

	private:
		template<typename ... TComponents>
		void SetComponentFlags(EntitySignature& flags) const { (flags.set(TComponents::GetComponentID()), ...); }

		template<typename ... TComponents, size_t ... Indices>
		View<TComponents...> CreateView(const EntitySignature& flags, std::index_sequence<Indices...>) const
		{
			return View<TComponents...>(CreateViewData<TComponents...>(std::make_index_sequence<sizeof ... (TComponents)>{}));
		}

		template<typename ... TComponents, size_t ... Indices>
		std::tuple<std::vector<TComponents>...> CreateViewData(std::index_sequence<Indices...>) const
		{
			return std::make_tuple(ConvertVectorContents<IComponent*, TComponents>(Components[Indices])...);
		}

		template<typename From, typename To>
		std::vector<To> ConvertVectorContents(const std::vector<From>& v) const
		{
			const size_t size{ v.size() };
			std::vector<To> vTo(size);

			for (size_t i{}; i < size; ++i)
			{
				vTo[i] = static_cast<To>(v[i]);
			}

			return vTo;
		}

		std::vector<EntitySignature> EntitySignatures;
		std::vector<Entity> Entities;
		std::vector<std::vector<IComponent*>> Components;
	};

	template<typename Component>
	void System::AddComponent(const Entity& entity)
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

	template<typename Component>
	Component* const System::GetComponent(const Entity& id) const
	{
		assert(id != InvalidEntityID);
		assert(Component::GetComponentID() < Components.size());
		assert(static_cast<size_t>(id) < Entities.size());

		return static_cast<Component*>(Components[Component::GetComponentID()][id]);
	}
}