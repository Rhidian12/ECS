#pragma once

#include "../ECSConstants.h"
#include "../SparseSet/SparseSet.h"
#include "../View/View.h"
#include "../ComponentIDGenerator/ComponentIDGenerator.h"
#include "../ComponentStorage/ComponentStorage.h"
#include "RegisteredTypes.h"

#include <assert.h> /* assert() */
#include <unordered_map> /* unordered_map */
#include <string> /* std::string */

namespace ECS
{
	class Registry final
	{
	public:
		Registry();
		~Registry();

		Registry(const Registry&) noexcept = delete;
		Registry(Registry&& other) noexcept;
		Registry& operator=(const Registry&) noexcept = delete;
		Registry& operator=(Registry&& other) noexcept;

		template<typename ... Ts>
		[[nodiscard]] View<Ts...> CreateView()
		{
			return View<Ts...>{ std::move(GetComponents<Ts...>()) };
		}

		template<typename ... Ts>
		std::tuple<Ts&...> AddComponents(const Entity entity)
		{
			std::unique_ptr<IComponentStorage>& pool{ ComponentPools[GetCombinedTypeName<Ts...>()] };

			if (!pool)
			{
				pool.reset(new ComponentStorage<Ts...>{});
			}

			return std::tuple<Ts&...>(static_cast<ComponentStorage<Ts...>*>(pool.get())->AddComponent<Ts>(entity)...);
		}

		template<typename ... Ts>
		std::tuple<Ts&...> GetComponent(const Entity entity)
		{
			assert(ComponentPools[GetCombinedTypeName<Ts...>()]);
			return std::tuple<Ts&...>(static_cast<ComponentStorage<Ts...>*>(ComponentPools[GetCombinedTypeName<Ts...>()].get())
				->GetComponent<Ts>(entity)...);
		}
		template<typename ... Ts>
		std::tuple<const Ts&...> GetComponent(const Entity entity) const
		{
			assert(ComponentPools[GetCombinedTypeName<Ts...>()]);
			return std::tuple<const Ts&...>(static_cast<ComponentStorage<Ts...>*>(ComponentPools[GetCombinedTypeName<Ts...>()].get())
				->GetComponent<Ts>(entity)...);
		}

		template<typename ... Ts>
		std::tuple<std::vector<Ts>&...> GetComponents()
		{
			assert(ComponentPools[GetCombinedTypeName<Ts...>()]);
			return std::tuple<std::vector<Ts>&...>(static_cast<ComponentStorage<Ts...>*>(ComponentPools[GetCombinedTypeName<Ts...>()].get())
				->GetComponents<Ts>()...);
		}
		template<typename ... Ts>
		std::tuple<const std::vector<Ts>&...> GetComponents() const
		{
			assert(ComponentPools[GetCombinedTypeName<Ts...>()]);
			return std::tuple<std::vector<Ts>&...>(static_cast<ComponentStorage<Ts...>*>(ComponentPools[GetCombinedTypeName<Ts...>()].get())
				->GetComponents<Ts>()...);
		}

		Entity CreateEntity();
		bool ReleaseEntity(Entity entity);
		Entity GetAmountOfEntities() const { return CurrentEntityCounter; }

	private:
		void RemoveAllComponents(const Entity entity);

		template<typename ... Ts>
		__forceinline std::string GetCombinedTypeName() { return (std::string{ Utils::ConstexprTypeName<Ts>() } + ...); }

		std::vector<size_t> ComponentIDs;
		SparseSet<Entity> Entities;
		Entity CurrentEntityCounter;
		std::unordered_map<std::string, std::unique_ptr<IComponentStorage>> ComponentPools;
	};
}