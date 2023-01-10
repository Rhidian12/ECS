#pragma once

#include "../ECSConstants.h"
#include "../SparseSet/SparseSet.h"

#include <vector> /* std::vector */
#include <map> /* std::map */

#ifdef OLD
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
		ComponentArray(const size_t nrOfComponentsPerList)
			: Entities{}
			, Components{}
			, NrOfComponentsPerList{ nrOfComponentsPerList }
			, NrOfEntities{}
		{}

		ComponentArray(const ComponentArray&) noexcept = delete;
		ComponentArray(ComponentArray&& other) noexcept
			: Entities{ std::move(other.Entities) }
			, Components{ std::move(other.Components) }
			, NrOfComponentsPerList{ std::move(other.NrOfComponentsPerList) }
			, NrOfEntities{ std::move(other.NrOfEntities) }
		{
			other.Components.clear();
			other.Entities = nullptr;
		}

		ComponentArray& operator=(const ComponentArray&) noexcept = delete;
		ComponentArray& operator=(ComponentArray&& other) noexcept
		{
			Entities = std::move(other.Entities);
			Components = std::move(other.Components);
			NrOfComponentsPerList = std::move(other.NrOfComponentsPerList);
			NrOfEntities = std::move(other.NrOfEntities);

			other.Entities = nullptr;
			other.Components.clear();

			return *this;
		};

		T& AddComponent(const Entity entity)
		{
			while (entity / NrOfComponentsPerList >= Components.size())
			{
				Entities.push_back(std::vector<Entity>{});
				Components.push_back(std::vector<T>{});
			}

			++NrOfEntities;

			Entities[entity / NrOfComponentsPerList].emplace_back(entity);
			return Components[entity / NrOfComponentsPerList].emplace_back(T{});
		}
		template<typename ... Ts>
		T& AddComponent(const Entity entity, Ts&& ... args)
		{
			while (entity / NrOfComponentsPerList >= Components.size())
			{
				Entities.push_back(std::vector<Entity>{});
				Components.push_back(std::vector<T>{});
			}

			++NrOfEntities;

			Entities[entity / NrOfComponentsPerList].emplace_back(entity);
			return Components[entity / NrOfComponentsPerList].emplace_back(T{ std::forward<Ts>(args)... });
		}

		virtual void Remove(const Entity entity) override
		{
			--NrOfEntities;

			Components[entity / NrOfComponentsPerList].erase(
				Components[entity / NrOfComponentsPerList].begin() +
				(std::find(
					Entities[entity / NrOfComponentsPerList].cbegin(),
					Entities[entity / NrOfComponentsPerList].cend(),
					entity) -
					Entities[entity / NrOfComponentsPerList].cbegin()));
		}

		T& GetComponent(const Entity entity)
		{
			return Components[entity / NrOfComponentsPerList][
				std::find(
					Entities[entity / NrOfComponentsPerList].cbegin(),
					Entities[entity / NrOfComponentsPerList].cend(),
					entity) -
					Entities[entity / NrOfComponentsPerList].cbegin()];
		}
		const T& GetComponent(const Entity entity) const
		{
			return Components[entity / NrOfComponentsPerList][
				std::find(
					Entities[entity / NrOfComponentsPerList].cbegin(),
					Entities[entity / NrOfComponentsPerList].cend(),
					entity) -
					Entities[entity / NrOfComponentsPerList].cbegin()];
		}

		std::vector<std::vector<T>>& GetComponents() { return Components; }
		const std::vector<std::vector<T>>& GetComponents() const { return Components; }

		std::vector<std::vector<Entity>>& GetEntities() { return Entities; }
		const std::vector<std::vector<Entity>>& GetEntities() const { return Entities; }

		size_t GetNrOfEntities() const { return NrOfEntities; }

	private:
		std::vector<std::vector<Entity>> Entities;
		std::vector<std::vector<T>> Components;
		size_t NrOfComponentsPerList;
		size_t NrOfEntities;
	};
}
#endif

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
			m_Entities.insert(std::make_pair(entity, static_cast<uint32_t>(m_Components.size())));
			m_IsSlotUsed.push_back(true);
			return m_Components.emplace_back(T{});
		}
		template<typename ... Ts>
		T& AddComponent(const Entity entity, Ts&& ... args)
		{
			m_Entities.insert(std::make_pair(entity, static_cast<uint32_t>(m_Components.size())));
			m_IsSlotUsed.push_back(true);
			return m_Components.emplace_back(T{ std::forward<Ts>(args)... });
		}

		virtual void Remove(const Entity entity) override
		{
			m_IsSlotUsed[m_Entities.at(entity)] = false;

			// m_Components.erase(m_Components.begin() + m_Entities.at(entity));
			// for (auto beg{ m_Entities.find(entity) }; beg != m_Entities.end(); ++beg)
			// 	--(beg->second);
		}

		T& GetComponent(const Entity entity)
		{
			assert(m_IsSlotUsed[m_Entities.at(entity)]);
			return m_Components[m_Entities.at(entity)];
		}
		const T& GetComponent(const Entity entity) const
		{
			assert(m_IsSlotUsed[m_Entities.at(entity)]);
			return m_Components[m_Entities.at(entity)];
		}

	private:
		std::vector<T> m_Components;
		std::map<Entity, uint32_t> m_Entities; // Copying all this extra data :/ Worries for later
		std::vector<bool> m_IsSlotUsed;
	};
}