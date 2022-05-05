#pragma once

#include "../ECSConstants.h"

#include <vector> /* std::vector */
#include <memory> /* std::unique_ptr */
#include <assert.h> /* assert() */
#include <algorithm> /* std::find, std::find_if, ... */

namespace ECS
{
	class IComponentArray
	{
	public:
		virtual ~IComponentArray() = default;
	};

	template<typename TComponent>
	class ComponentArray final : public IComponentArray
	{
	public:
		void AddComponent(const Entity entity)
		{
			static_assert(std::is_default_constructible_v<TComponent>, "Component must be default constructible");

			Components.push_back(TComponent());
		}
		template<typename ... Values>
		void AddComponent(const Entity entity, Values&&... values)
		{
			Components.push_back(TComponent(std::forward<Values>(values)...));
		}

		TComponent& GetComponent(const Entity entity)
		{
		}
		const TComponent& GetComponent(Entity entity) const { assert(entity < Components.size()); return Components[entity]; }

		std::vector<TComponent>& GetComponents() { return Components; }
		const std::vector<TComponent>& GetComponents() const { return Components; }

	private:
		std::vector<TComponent> Components;
	};

	class ComponentManager final
	{
	public:
		static ComponentManager* GetInstance();

		ComponentManager(const ComponentManager&) noexcept = delete;
		ComponentManager(ComponentManager&&) noexcept = delete;
		ComponentManager& operator=(const ComponentManager&) noexcept = delete;
		ComponentManager& operator=(ComponentManager&&) noexcept = delete;

		template<typename TComponent>
		void AddComponent(Entity entity)
		{
			const ComponentType componentID{ TComponent::GetComponentID() };

			if (componentID >= ComponentArrays.size())
			{
				ComponentArrays.resize(componentID + 1);
			}

			std::unique_ptr<IComponentArray>& pArray(ComponentArrays[componentID]);
			if (!pArray)
			{
				pArray = std::make_unique<ComponentArray<TComponent>>();
			}

			static_cast<ComponentArray<TComponent>*>(pArray.get())->AddComponent(entity);
		}
		template<typename TComponent, typename ... Values>
		void AddComponent(Entity entity, Values&&... values)
		{
			const ComponentType componentID{ TComponent::GetComponentID() };

			if (componentID >= ComponentArrays.size())
			{
				ComponentArrays.resize(componentID + 1);
			}

			std::unique_ptr<IComponentArray>& pArray(ComponentArrays[componentID]);
			if (!pArray)
			{
				pArray = std::make_unique<ComponentArray<TComponent>>();
			}

			static_cast<ComponentArray<TComponent>*>(pArray.get())->AddComponent(entity, values...);
		}

		template<typename TComponent>
		TComponent& GetComponent(Entity entity) { assert(TComponent::GetComponentID() < ComponentArrays.size()); return static_cast<ComponentArray<TComponent>*>(ComponentArrays[TComponent::GetComponentID()].get())->GetComponent(entity); }
		template<typename TComponent>
		const TComponent& GetComponent(Entity entity) const { assert(TComponent::GetComponentID() < ComponentArrays.size()); return static_cast<ComponentArray<TComponent>*>(ComponentArrays[TComponent::GetComponentID()].get())->GetComponent(entity); }

		template<typename TComponent>
		std::vector<TComponent>& GetComponents() { assert(TComponent::GetComponentID() < ComponentArrays.size()); return static_cast<ComponentArray<TComponent>*>(ComponentArrays[TComponent::GetComponentID()].get())->GetComponents(); }
		template<typename TComponent>
		const std::vector<TComponent>& GetComponents() const { assert(TComponent::GetComponentID() < ComponentArrays.size()); return static_cast<ComponentArray<TComponent>*>(ComponentArrays[TComponent::GetComponentID()].get())->GetComponents(); }

	private:
		ComponentManager() = default;

		friend std::unique_ptr<ComponentManager> std::make_unique<ComponentManager>();

		inline static std::unique_ptr<ComponentManager> Instance{};

		std::vector<std::unique_ptr<IComponentArray>> ComponentArrays;
	};
}