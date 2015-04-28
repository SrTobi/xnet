#pragma once
#ifndef _XNET_SERIALIZATION_TYPE_LIST_HPP
#define _XNET_SERIALIZATION_TYPE_LIST_HPP


#include <list>
#include "../serialization.hpp"
#include "detail/serialize_sequence.hpp"

namespace xnet {
	namespace serialization {
		namespace detail {
			template<typename Sink, typename Context, typename T>
			void split_serialize_list(serializer<Sink, Context>& s, const std::list<T>& v)
			{
				detail::save_sequence(s, v.begin(), v.end(), v.size(), nullptr);
			}

			template<typename Source, typename Context, typename T>
			void split_serialize_list(deserializer<Source, Context>& s, std::list<T>& v)
			{
				auto size = s.begin_sequence_load(nullptr);
				v.clear();
				while(size--)
				{
					v.emplace_back();
					s >> v.back();
				}
				s.end_sequence_load(nullptr);
			}
		}


		template<typename S, typename T>
		void serialize(S& s, std::list<T>& v)
		{
			s.current_type("std::list");
			detail::split_serialize_list(s, v);
		}
	}
}


#endif