#pragma once

#include "../ECSConstants.h"
#include "../SparseSet/SparseSet.h"

#include <vector> /* std::vector */
#include <memory> /* std::unique_ptr */
#include <assert.h> /* assert() */
#include <algorithm> /* std::find, std::find_if, ... */
#include <unordered_map>

namespace ECS
{
	class IComponentArray
	{
	public:
		virtual ~IComponentArray() = default;

		virtual void Remove(const Entity entity) = 0;
	};

	template<typename T>
	class ComponentArray final : public IComponentArray
	{
	public:
		T& AddComponent(const Entity entity)
		{
			assert(!Entities.Contains(entity));

			Entities.Add(entity);
			return Components.emplace_back(T{});
		}
		/* [TODO]: Make variadic template overload */

		virtual void Remove(const Entity entity) override
		{
			assert(Entities.Contains(entity));

			Components.erase(Components.begin() + Entities.GetIndex(entity));
			Entities.Remove(entity);
		}

		bool IsEntityAdded(const Entity entity) const { return Entities.Contains(entity); }

		T& GetComponent(const Entity entity) { assert(Entities.Contains(entity)); return Components[Entities.GetIndex(entity)]; }
		const T& GetComponent(const Entity entity) const { assert(Entities.Contains(entity)); return Components[Entities.GetIndex(entity)]; }

		std::vector<T>& GetComponents() { return Components; }
		const std::vector<T>& GetComponents() const { return Components; }

	private:
		SparseSet<Entity> Entities;
		std::vector<T> Components;
	};

	class ComponentManager
	{
	public:
		static ComponentManager& GetInstance();

		ComponentManager(const ComponentManager&) noexcept = delete;
		ComponentManager(ComponentManager&&) noexcept = delete;
		ComponentManager& operator=(const ComponentManager&) noexcept = delete;
		ComponentManager& operator=(ComponentManager&&) noexcept = delete;

		template<typename T>
		T& AddComponent(const Entity entity)
		{
			std::unique_ptr<IComponentArray>& pool{ ComponentPools[GenerateComponentID<T>()] };

			if (!pool)
			{
				pool.reset(new ComponentArray<T>{});
			}

			return static_cast<ComponentArray<T>*>(pool.get())->AddComponent(entity);
		}
		/* [TODO]: Make variadic template overload */

		template<typename T>
		void RemoveComponent(const Entity entity)
		{
			assert(ComponentPools[GenerateComponentID<T>()]);
			ComponentPools[GenerateComponentID<T>()]->Remove(entity);
		}
		void RemoveAllComponents(const Entity entity, const EntitySignature& sig)
		{
			for (ComponentType i{}; i < MaxComponentTypes; ++i)
			{
				if (sig.test(i))
				{
					assert(ComponentPools[i]);

					ComponentPools[i]->Remove(entity);
				}
			}
		}

		template<typename T>
		T& GetComponent(const Entity entity)
		{
			assert(ComponentPools[GenerateComponentID<T>()]);
			assert(static_cast<ComponentArray<T>*>(ComponentPools[GenerateComponentID<T>()].get())->IsEntityAdded(entity));
			return static_cast<ComponentArray<T>*>(ComponentPools[GenerateComponentID<T>()].get())->GetComponent(entity);
		}
		template<typename T>
		const T& GetComponent(const Entity entity) const
		{
			assert(ComponentPools[GenerateComponentID<T>()]);
			assert(static_cast<ComponentArray<T>*>(ComponentPools[GenerateComponentID<T>()].get())->IsEntityAdded(entity));
			return static_cast<ComponentArray<T>*>(ComponentPools[GenerateComponentID<T>()].get())->GetComponent<T>(entity);
		}

		template<typename T>
		std::vector<T>& GetComponents()
		{
			assert(ComponentPools[GenerateComponentID<T>()]);
			return static_cast<ComponentArray<T>*>(ComponentPools[GenerateComponentID<T>()].get())->GetComponents();
		}
		template<typename T>
		const std::vector<T>& GetComponents() const
		{
			assert(ComponentPools[GenerateComponentID<T>()]);
			return static_cast<ComponentArray<T>*>(ComponentPools[GenerateComponentID<T>()].get())->GetComponents();
		}

	private:
		ComponentManager() = default;

		friend std::unique_ptr<ComponentManager> std::make_unique<ComponentManager>();
		inline static std::unique_ptr<ComponentManager> Instance{};

		std::unordered_map<ComponentType, std::unique_ptr<IComponentArray>> ComponentPools;
	};

	//class IComponentArray
	//{
	//public:
	//	virtual ~IComponentArray() = default;

	//	virtual void RemoveComponent(const Entity) = 0;
	//};

	//template<typename TComponent>
	//class ComponentArray final : public IComponentArray
	//{
	//public:
	//	ComponentArray()
	//		: Components{}
	//		, ComponentIndices{}
	//		, Entities{}
	//	{
	//		ComponentIndices.reserve(MaxEntities);
	//		ComponentIndices.resize(MaxEntities, InvalidEntityID);

	//		Components.reserve(MaxEntities);
	//	}

	//	void AddComponent(const Entity entity)
	//	{
	//		static_assert(std::is_default_constructible_v<TComponent>, "Component must be default constructible");

	//		if (Entities.Add(entity))
	//		{
	//			ComponentIndices[entity] = static_cast<Entity>(Components.size());
	//			Components.push_back(TComponent());
	//		}
	//	}
	//	template<typename ... Values>
	//	void AddComponent(const Entity entity, Values&&... values)
	//	{
	//		if (Entities.Add(entity))
	//		{
	//			ComponentIndices[entity] = static_cast<Entity>(Components.size());
	//			Components.push_back(TComponent(values...));
	//		}
	//	}

	//	virtual void RemoveComponent(const Entity entity) override
	//	{
	//		if (Entities.Contains(entity))
	//		{
	//			if (Entities.Remove(entity))
	//			{
	//				Components.erase(Components.begin() + ComponentIndices[entity]);
	//				ComponentIndices[entity] = InvalidEntityID;

	//				for (Entity i(entity + static_cast<Entity>(1u)); i < ComponentIndices.size(); ++i)
	//				{
	//					if (ComponentIndices[i] != 0 && ComponentIndices[i] != InvalidEntityID)
	//					{
	//						--ComponentIndices[i];
	//					}
	//				}
	//			}
	//		}
	//	}

	//	TComponent& GetComponent(const Entity entity)
	//	{
	//		assert(Entities.Contains(entity) && "ComponentArray::GetComponent() > This entity does not have the corresponding component!");

	//		return Components[entity];
	//	}
	//	const TComponent& GetComponent(const Entity entity) const
	//	{
	//		assert(Entities.Contains(entity) && "ComponentArray::GetComponent() > This entity does not have the corresponding component!");

	//		return Components[entity];
	//	}

	//	std::vector<TComponent>& GetComponents() { return Components; }
	//	const std::vector<TComponent>& GetComponents() const { return Components; }

	//	Entity GetNrOfComponents() const { return static_cast<Entity>(Components.size()); }

	//private:
	//	std::vector<TComponent> Components;
	//	std::vector<Entity> ComponentIndices;
	//	SparseSet<Entity> Entities;
	//};

	//class ComponentManager final
	//{
	//public:
	//	static ComponentManager* GetInstance();

	//	ComponentManager(const ComponentManager&) noexcept = delete;
	//	ComponentManager(ComponentManager&&) noexcept = delete;
	//	ComponentManager& operator=(const ComponentManager&) noexcept = delete;
	//	ComponentManager& operator=(ComponentManager&&) noexcept = delete;

	//	template<typename TComponent>
	//	void AddComponent(Entity entity)
	//	{
	//		const ComponentType componentID{ TComponent::GetComponentID() };

	//		if (componentID >= ComponentArrays.size())
	//		{
	//			ComponentArrays.resize(componentID + 1);
	//		}

	//		std::unique_ptr<IComponentArray>& pArray(ComponentArrays[componentID]);
	//		if (!pArray)
	//		{
	//			pArray = std::make_unique<ComponentArray<TComponent>>();
	//		}

	//		static_cast<ComponentArray<TComponent>*>(pArray.get())->AddComponent(entity);
	//	}
	//	template<typename TComponent, typename ... Values>
	//	void AddComponent(Entity entity, Values&&... values)
	//	{
	//		const ComponentType componentID{ TComponent::GetComponentID() };

	//		if (componentID >= ComponentArrays.size())
	//		{
	//			ComponentArrays.resize(componentID + 1);
	//		}

	//		std::unique_ptr<IComponentArray>& pArray(ComponentArrays[componentID]);
	//		if (!pArray)
	//		{
	//			pArray = std::make_unique<ComponentArray<TComponent>>();
	//		}

	//		static_cast<ComponentArray<TComponent>*>(pArray.get())->AddComponent(entity, values...);
	//	}

	//	void RemoveAllComponents(Entity entity, EntitySignature id)
	//	{
	//		for (ComponentType i{}; i < MaxComponentTypes; ++i)
	//		{
	//			if (id.test(i))
	//			{
	//				assert(i < ComponentArrays.size());

	//				if (ComponentArrays[i])
	//				{
	//					ComponentArrays[i]->RemoveComponent(entity);
	//				}
	//			}
	//		}
	//	}

	//	void RemoveComponent(const Entity entity, const ComponentType componentID)
	//	{
	//		assert(componentID != InvalidComponentID);

	//		if (ComponentArrays[componentID])
	//		{
	//			ComponentArrays[componentID]->RemoveComponent(entity);
	//		}
	//	}

	//	template<typename TComponent>
	//	TComponent& GetComponent(Entity entity) { assert(TComponent::GetComponentID() < ComponentArrays.size()); return static_cast<ComponentArray<TComponent>*>(ComponentArrays[TComponent::GetComponentID()].get())->GetComponent(entity); }
	//	template<typename TComponent>
	//	const TComponent& GetComponent(Entity entity) const { assert(TComponent::GetComponentID() < ComponentArrays.size()); return static_cast<ComponentArray<TComponent>*>(ComponentArrays[TComponent::GetComponentID()].get())->GetComponent(entity); }

	//	template<typename TComponent>
	//	std::vector<TComponent>& GetComponents() { assert(TComponent::GetComponentID() < ComponentArrays.size()); return static_cast<ComponentArray<TComponent>*>(ComponentArrays[TComponent::GetComponentID()].get())->GetComponents(); }
	//	template<typename TComponent>
	//	const std::vector<TComponent>& GetComponents() const { assert(TComponent::GetComponentID() < ComponentArrays.size()); return static_cast<ComponentArray<TComponent>*>(ComponentArrays[TComponent::GetComponentID()].get())->GetComponents(); }

	//	template<typename TComponent>
	//	Entity GetNrOfComponents() const { assert(TComponent::GetComponentID() < ComponentArrays.size()); return static_cast<ComponentArray<TComponent>*>(ComponentArrays[TComponent::GetComponentID()].get())->GetNrOfComponents(); }

	//private:
	//	ComponentManager() = default;

	//	friend std::unique_ptr<ComponentManager> std::make_unique<ComponentManager>();

	//	inline static std::unique_ptr<ComponentManager> Instance{};

	//	std::vector<std::unique_ptr<IComponentArray>> ComponentArrays{};
	//};
}