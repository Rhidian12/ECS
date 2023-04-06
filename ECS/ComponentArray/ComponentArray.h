#pragma once

#include "../ECSConstants.h"
#include "../SparseSet/SparseSet.h"

#include "../Benchmark/BenchmarkUtils.h"

#include <vector> /* std::vector */

namespace ECS
{
	class IComponentArray
	{
	public:
		virtual ~IComponentArray() = default;

		virtual void Remove(const Entity entity) = 0;
		virtual void RemoveAll() = 0;
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
			PROFILE_SCOPE()

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
				m_Entities[entity] = InvalidEntityID;
			}
		}

		virtual void RemoveAll() override
		{
			m_Entities.Clear();
			m_Components.clear();
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