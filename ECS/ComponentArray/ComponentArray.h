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

		virtual void SetEntities(std::vector<std::vector<Entity>>* const entities) = 0;
	};

	template<typename T>
	class ComponentArray final : public IComponentArray
	{
	public:
		ComponentArray(std::vector<std::vector<Entity>>* const entities, const size_t nrOfComponentsPerList)
			: Entities{ entities }
			, Components{}
			, NrOfComponentsPerList{ nrOfComponentsPerList }
		{}

		ComponentArray(const ComponentArray&) noexcept = delete;
		ComponentArray(ComponentArray&& other) noexcept
			: Entities{ std::move(other.Entities) }
			, Components{ std::move(other.Components) }
			, NrOfComponentsPerList{std::move(other.NrOfComponentsPerList)}
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

			other.Entities = nullptr;
			other.Components.clear();

			return *this;
		};

		T& AddComponent(const Entity entity)
		{
			// return Components.Add(entity, T{});
			while (entity / NrOfComponentsPerList >= Components.size())
			{
				Components.push_back(std::vector<T>{});
			}

			return Components[entity / NrOfComponentsPerList].emplace_back(T{});
		}
		template<typename ... Ts>
		T& AddComponent(const Entity entity, Ts&& ... args)
		{
			// return Components.Add(entity, T{ std::forward<Ts>(args)... });
			while (entity / NrOfComponentsPerList >= Components.size())
			{
				Components.push_back(std::vector<T>{});
			}

			return Components[entity / NrOfComponentsPerList].emplace_back(T{ std::forward<Ts>(args)... });
		}

		//bool HasComponent(const Entity entity) const
		//{
		//	// return Components.ContainsKey(entity);
		//	return Components[entity / NrOfComponentsPerList].ContainsKey(entity);
		//}

		virtual void Remove(const Entity entity) override
		{
			// Components.Remove(entity);
			Components[entity / NrOfComponentsPerList].erase(
				Components[entity / NrOfComponentsPerList].begin() +
				(std::find(
					(*Entities)[entity / NrOfComponentsPerList].cbegin(),
					(*Entities)[entity / NrOfComponentsPerList].cend(),
					entity) - 
				(*Entities)[entity / NrOfComponentsPerList].cbegin()));
		}

		// void SafeRemove(const Entity entity)
		// {
		// 	Components.SafeRemove(entity);
		// }

		//T& GetComponent(const Entity entity) { return Components.GetValue(entity); }
		//const T& GetComponent(const Entity entity) const { return Components.GetValue(entity); }

		//std::vector<T>& GetComponents() { return Components.GetValues(); }
		//const std::vector<T>& GetComponents() const { return Components.GetValues(); }

		//std::vector<Entity>& GetKeys() { return Components.GetKeys(); }
		//const std::vector<Entity>& GetKeys() const { return Components.GetKeys(); }

		//DoubleStorage<Entity, T>& GetStorage() { return Components; }
		//const DoubleStorage<Entity, T>& GetStorage() const { return Components; }

		T& GetComponent(const Entity entity)
		{
			return Components[entity / NrOfComponentsPerList][
				std::find(
					(*Entities)[entity / NrOfComponentsPerList].cbegin(),
					(*Entities)[entity / NrOfComponentsPerList].cend(),
					entity) -
				(*Entities)[entity / NrOfComponentsPerList].cbegin()];
		}
		const T& GetComponent(const Entity entity) const
		{
			return Components[entity / NrOfComponentsPerList][
				std::find(
					(*Entities)[entity / NrOfComponentsPerList].cbegin(),
					(*Entities)[entity / NrOfComponentsPerList].cend(),
					entity) -
					(*Entities)[entity / NrOfComponentsPerList].cbegin()];
		}

		std::vector<std::vector<T>>& GetComponents() { return Components; }
		const std::vector<std::vector<T>>& GetComponents() const { return Components; }

		virtual void SetEntities(std::vector<std::vector<Entity>>* const entities) override
		{
			Entities = entities;
		}

	private:
		// DoubleStorage<Entity, T> Components;
		// SparseSet<Entity>* Entities;
		std::vector<std::vector<Entity>>* Entities;
		std::vector<std::vector<T>> Components;
		size_t NrOfComponentsPerList;
	};
}