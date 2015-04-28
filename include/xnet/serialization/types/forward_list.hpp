#pragma once
#ifndef _XNET_SERIALIZATION_TYPE_FORWARDLIST_HPP
#define _XNET_SERIALIZATION_TYPE_FORWARDLIST_HPP


#include <forward_list>
#include "../serialization.hpp"
#include "detail/serialize_sequence.hpp"

namespace xnet {
	namespace serialization {
		namespace detail {
			template<typename Sink, typename Context, typename T>
			void split_serialize_forward_list(serializer<Sink, Context>& s, const std::forward_list<T>& v)
			{
				detail::save_sequence(s, v.begin(), v.end(), std::distance(v.cbegin(), v.cend()), nullptr);
			}

			template<typename Source, typename Context, typename T>
			void split_serialize_forward_list(deserializer<Source, Context>& s, std::forward_list<T>& v)
			{
				auto size = s.begin_sequence_load(nullptr);
				v.clear();
				auto it = v.before_begin();
				while(size--)
				{
					it = v.emplace_after(it);
					s >> *it;
				}
				s.end_sequence_load(nullptr);
			}
		}


		template<typename S, typename T>
		void serialize(S& s, std::forward_list<T>& v)
		{
			s.current_type("std::forward_list");
			detail::split_serialize_forward_list(s, v);
		}
	}
}


#endif