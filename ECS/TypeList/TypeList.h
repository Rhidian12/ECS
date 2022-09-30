#pragma once

namespace ECS
{
	// Reference: https://stackoverflow.com/questions/18701798/building-and-accessing-a-list-of-types-at-compile-time

	struct Void {};

	template<typename ...> struct Concatenate;

	template<template<typename  ...> typename List, typename T>
	struct Concatenate<List<Void>, T>
	{
		using Type = List<T>;
	};

	template<template<typename  ...> typename List, typename ... Ts, typename T>
	struct Concatenate<List<Ts...>, T>
	{
		using Type = List<Ts..., T>;
	};

	template<typename ...>
	struct TypeList {};

	template<>
	struct TypeList<Void> {};

	template<typename T>
	struct TypeListLength;

	template<typename ... Ts>
	struct TypeListLength<TypeList<Ts...>>
	{
		static constexpr size_t value = sizeof ... (Ts);
	};

	template<typename T, size_t I>
	struct TypeAt;

	template<typename T, typename ... Ts>
	struct TypeAt<TypeList<T, Ts...>, 0>
	{
		using type = T;
	};

	template<typename T, size_t I, typename ... Ts>
	struct TypeAt<TypeList<T, Ts...>, I>
	{
		static_assert(I < sizeof ... (Ts) + 1, "TypeList::TypeAt Index out of range");
		using type = typename TypeAt<TypeList<Ts...>, I - 1>::type;
	};

	using TypeListVoid = TypeList<Void>;

#define TYPE_LIST TypeListVoid
}