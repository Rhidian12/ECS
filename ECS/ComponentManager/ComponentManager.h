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
		template<typename ... Ts>
		T& AddComponent(const Entity entity, Ts&& ... args)
		{
			assert(!Entities.Contains(entity));

			Entities.Add(entity);
			return Components.emplace_back(T{ args... });
		}

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

	class ComponentManager final
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
		template<typename T, typename ... Ts>
		T& AddComponent(const Entity entity, Ts&& ... args)
		{
			std::unique_ptr<IComponentArray>& pool{ ComponentPools[GenerateComponentID<T>()] };

			if (!pool)
			{
				pool.reset(new ComponentArray<T>{});
			}

			return static_cast<ComponentArray<T>*>(pool.get())->AddComponent<Ts...>(entity, args...);
		}

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
}