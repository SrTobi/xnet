#pragma once
#ifndef _XNET_SERIALIZATION_TYPE_TUPLE_HPP
#define _XNET_SERIALIZATION_TYPE_TUPLE_HPP

#include <tuple>
#include "../serialization.hpp"

namespace xnet {
	namespace serialization {

		namespace detail {
			template<typename Tuple, size_t Index>
			struct tuple_serializer
			{
				template<typename S>
				static void serialize(S& s, Tuple& t)
				{
					s & (std::get<std::tuple_size<Tuple>::value - Index>(t));
					tuple_serializer<Tuple, Index - 1>::serialize(s, t);
				}
			};

			template<typename Tuple>
			struct tuple_serializer<Tuple, 0>
			{
				template<typename S>
				static void serialize(S& s, const Tuple&)
				{
				}
			};
		}

		template<typename S, typename... Args>
		void serialize(S& s, std::tuple<Args...>& t)
		{
			typedef std::tuple<Args...> ttype;
			s.current_type("std::tuple");
			detail::tuple_serializer<ttype, std::tuple_size<ttype>::value>::serialize(s, t);
		}
	}
}


#endif