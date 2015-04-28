#pragma once
#ifndef _XNET_DETAIL_VARIADIC_UTILS_HPP
#define _XNET_DETAIL_VARIADIC_UTILS_HPP

#include <type_traits>


namespace xnet {
	namespace detail {
		/************************************** Variadic and **************************************/
		template<bool L, bool... tail>
		struct variadic_and
			: public std::false_type
		{
		};

		template<>
		struct variadic_and<true>
			: public std::true_type
		{
		};

		template<bool... tail>
		struct variadic_and<true, tail...>
			: public variadic_and<tail...>
		{
		};


		/************************************** Variadic or **************************************/
		template<bool L, bool... tail>
		struct variadic_or
			: public std::true_type
		{
		};

		template<>
		struct variadic_or<false>
			: public std::false_type
		{
		};

		template<bool... tail>
		struct variadic_or<false, tail...>
			: public variadic_and<tail...>
		{
		};


		/************************************** is_specialization  **************************************/
		template<typename Test, template<typename...> class Ref>
		struct is_specialization : public std::false_type {};

		template<template<typename...> class Ref, typename... Args>
		struct is_specialization<Ref<Args...>, Ref> : public std::true_type {};
	}
}


#endif