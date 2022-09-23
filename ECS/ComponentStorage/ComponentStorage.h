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
	};

	template <typename...> struct TypeList;

	template <typename T, typename... Ts>
	struct TypeList<T, Ts...>
	{
		using Head = T;
		using Tail = TypeList<Ts...>;
	};

	template <typename T> struct Length;

	template <typename... Ts>
	struct Length<TypeList<Ts...>>
	{
		static constexpr std::size_t Value = sizeof...(Ts);
	};

	template <typename T>
	inline constexpr std::size_t Length_v = Length<T>::Value;

	template <typename T, std::size_t Index> struct TypeAt;

	template <typename T, typename... Ts>
	struct TypeAt<TypeList<T, Ts...>, 0>
	{
		using Type = T;
	};

	template <typename T, typename... Ts, std::size_t Index>
	struct TypeAt<TypeList<T, Ts...>, Index>
	{
		static_assert(Index < sizeof...(Ts) + 1, "index out of range");
		using Type = typename TypeAt<TypeList<Ts...>, Index - 1>::Type;
	};

	template <typename T, std::size_t Index>
	using TypeAt_t = typename TypeAt<T, Index>::Type;

	template<typename ... Ts>
	class ComponentStorage final : public IComponentStorage
	{
	public:
		template<typename T>
		void AddComponent(const Entity entity)
		{
			std::get<Index<T, Storage>::value>(Storage).Add(entity, T{});
		}
		template<typename T, typename ... Us>
		void AddComponent(const Entity entity, Us&& ... args)
		{
			std::get<Index<T, Storage>::value>(Storage).Add(entity, T{ std::forward<Us>(args)... });
		}

		template<typename T>
		T& GetComponent(const Entity entity)
		{
			std::get<Index<T, Storage>::value>(Storage).GetValue(entity);
		}
		template<typename T>
		const T& GetComponent(const Entity entity) const
		{
			std::get<Index<T, Storage>::value>(Storage).GetValue(entity);
		}

		template<typename T>
		std::vector<T>& GetComponents()
		{
			std::get<Index<T, Storage>::value>(Storage).GetValues();
		}
		template<typename T>
		const std::vector<T>& GetComponents() const
		{
			std::get<Index<T, Storage>::value>(Storage).GetValues();
		}

		using Types = TypeList<Ts...>;

	private:
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