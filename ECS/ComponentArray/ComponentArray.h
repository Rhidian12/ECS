#pragma once

#include <vector> /* std::vector */

#include "../ECSConstants.h"

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
		T& AddComponent()
		{
			return Components.emplace_back(T{});
		}
		template<typename ... Ts>
		T& AddComponent(Ts&& ... args)
		{
			return Components.emplace_back(T{ args... });
		}

		virtual void Remove(const Entity entity) override
		{
			Components.erase(Components.begin() + entity);
		}

		T& GetComponent(const Entity entity) { return Components[entity]; }
		const T& GetComponent(const Entity entity) const { return Components[entity]; }

		std::vector<T>& GetComponents() { return Components; }
		const std::vector<T>& GetComponents() const { return Components; }

	private:
		std::vector<T> Components;
	};
}