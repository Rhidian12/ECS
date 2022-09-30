#pragma once
#include "../ECSConstants.h"

#include "../DoubleStorage/DoubleStorage.h"

#include <tuple> /* std::tuple */

namespace ECS
{
	class IComponentStorage
	{
	public:
		virtual ~IComponentStorage() = default;

		virtual void Remove(const Entity) = 0;

		virtual bool HasEntity(const Entity) const = 0;
	};

	template<typename ... Ts>
	class ComponentStorage final : public IComponentStorage
	{
	public:
		template<typename T>
		T& AddComponent(const Entity entity)
		{
			return std::get<Index<DoubleStorage<Entity, T>, decltype(Storage)>::value>(Storage).Add(entity, T{});
		}
		template<typename T, typename ... Us>
		T& AddComponent(const Entity entity, Us&& ... args)
		{
			return std::get<Index<DoubleStorage<Entity, T>, decltype(Storage)>::value>(Storage).Add(entity, T{ std::forward<Us>(args)... });
		}

		template<typename T>
		T& GetComponent(const Entity entity)
		{
			return std::get<Index<DoubleStorage<Entity, T>, decltype(Storage)>::value>(Storage).GetValue(entity);
		}
		template<typename T>
		const T& GetComponent(const Entity entity) const
		{
			return std::get<Index<DoubleStorage<Entity, T>, decltype(Storage)>::value>(Storage).GetValue(entity);
		}

		template<typename T>
		std::vector<T>& GetComponents()
		{
			return std::get<Index<DoubleStorage<Entity, T>, decltype(Storage)>::value>(Storage).GetValues();
		}
		template<typename T>
		const std::vector<T>& GetComponents() const
		{
			return std::get<Index<DoubleStorage<Entity, T>, decltype(Storage)>::value>(Storage).GetValues();
		}

		virtual void Remove(const Entity entity) override
		{
			// RemoveImpl<0>(entity);

			std::apply([entity](auto&& ... args) { (args.Remove(entity), ...); }, Storage);
		}

		virtual bool HasEntity(const Entity entity) const override
		{
			return std::get<0>(Storage).ContainsKey(entity);
		}

	private:
		template<size_t I = 0, std::enable_if_t<I == sizeof ... (Ts), void>>
		void RemoveImpl(const Entity) {}

		template<size_t I = 0, std::enable_if_t<I < sizeof ... (Ts), void>>
		void RemoveImpl(const Entity entity)
		{
			std::get<I>(Storage).Remove(entity);

			Remove<I + 1>(entity);
		}

		std::tuple<DoubleStorage<Entity, Ts>...> Storage;

		template <class T, class Tuple>
		struct Index;

		template <class T, class... Types>
		struct Index<T, std::tuple<T, Types...>>
		{
			static const std::size_t value = 0;
		};

		template <class T, class U, class... Types>
		struct Index<T, std::tuple<U, Types...>>
		{
			static const std::size_t value = 1 + Index<T, std::tuple<Types...>>::value;
		};
	};
}