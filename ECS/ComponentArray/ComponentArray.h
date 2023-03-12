#pragma once

#include "../ECSConstants.h"
#include "../SparseSet/SparseSet.h"

#include <vector> /* std::vector */

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
		ComponentArray() = default;

		ComponentArray(const ComponentArray&) noexcept = delete;
		ComponentArray(ComponentArray&&) noexcept = default;
		ComponentArray& operator=(const ComponentArray&) noexcept = delete;
		ComponentArray& operator=(ComponentArray&&) noexcept = default;

		T& AddComponent(const Entity entity)
		{
			m_Entities.Add(entity);
			return m_Components.emplace_back(T{});
		}
		template<typename ... Ts>
		T& AddComponent(const Entity entity, Ts&& ... args)
		{
			m_Entities.Add(entity);
			return m_Components.emplace_back(T{ std::forward<Ts>(args)... });
		}

		virtual void Remove(const Entity entity) override
		{
			if (m_Entities.Contains(entity))
			{
				// problem: When we add components, the id's in m_Entities point to the correct spot in m_Components,
				// however the moment we remove a component, everything after that removed component is no longer in the correct
				// spot. How do I fix this?
				
				// m_Components.erase(m_Components.begin() + m_Entities[entity]);
				// m_Entities.Remove(entity);
			}
		}

		T& GetComponent(const Entity entity)
		{
			return m_Components[m_Entities[entity]];
		}
		const T& GetComponent(const Entity entity) const
		{
			return m_Components[m_Entities[entity]];
		}

	private:
		SparseSet<Entity> m_Entities;
		std::vector<T> m_Components;
	};
}