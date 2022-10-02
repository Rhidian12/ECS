#pragma once

#include <vector> /* std::vector */

#include "../ECSConstants.h"
#include "../DoubleStorage/DoubleStorage.h"

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
			return Components.Add(entity, T{});
		}
		template<typename ... Ts>
		T& AddComponent(const Entity entity, Ts&& ... args)
		{
			return Components.Add(entity, T{ std::forward<Ts>(args)... });
		}

		bool HasComponent(const Entity entity) const
		{
			return Components.ContainsKey(entity);
		}

		virtual void Remove(const Entity entity) override
		{
			Components.Remove(entity);
		}

		void SafeRemove(const Entity entity)
		{
			Components.SafeRemove(entity);
		}

		T& GetComponent(const Entity entity) { return Components.GetValue(entity); }
		const T& GetComponent(const Entity entity) const { return Components.GetValue(entity); }

		std::vector<T>& GetComponents() { return Components.GetValues(); }
		const std::vector<T>& GetComponents() const { return Components.GetValues(); }

		std::vector<Entity>& GetKeys() { return Components.GetKeys(); }
		const std::vector<Entity>& GetKeys() const { return Components.GetKeys(); }

	private:
		DoubleStorage<Entity, T> Components;
	};
}