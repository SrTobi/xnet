#pragma once
#ifndef _XNET_SERIALIZATION_TYPE_ENUM_HPP
#define _XNET_SERIALIZATION_TYPE_ENUM_HPP

#include <utility>
#include "../serialization.hpp"
#include "../split.hpp"

namespace xnet {
	namespace serialization {
		namespace detail {
			template<typename Sink, typename Context, typename E>
			void split_serialize_enum(serializer<Sink, Context>& s, const E& e)
			{
				static_assert(std::is_enum<E>::value, "E is not an enum!");
				s << std::underlying_type<E>::type(e);
			}

			template<typename Source, typename Context, typename E>
			void split_serialize_enum(deserializer<Source, Context>& s, E& e)
			{
				static_assert(std::is_enum<E>::value, "E is not an enum!");
				std::underlying_type<E>::type val;
				s >> val;
				e = E(val);
			}
		}

		template<typename S, typename E>
		void serialize_enum(S& s, E& e, const char* type = "enum")
		{
			static_assert(std::is_enum<E>::value, "E is not an enum!");
			s.current_type(type);
			detail::split_serialize_enum(s, e);
		}

#define XNET_SERIALIZATION_ENUM_NAMED(etype, ename) \
	template<typename S> void serialize(S& s, etype& e){ ::xnet::serialization::serialize_enum(s, e, ename);}

#define XNET_SERIALIZATION_ENUM(etype) \
	XNET_SERIALIZATION_ENUM_NAMED(etype, #etype)
	}
}


#endif