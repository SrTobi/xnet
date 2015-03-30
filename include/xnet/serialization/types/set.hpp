#pragma once
#ifndef _XNET_SERIALIZATION_TYPE_SET_HPP
#define _XNET_SERIALIZATION_TYPE_SET_HPP


#include <set>
#include "../serialization.hpp"
#include "detail/serialize_sequence.hpp"

namespace xnet {
	namespace serialization {
		namespace detail {
			template<typename Sink, typename T>
			void split_serialize_set(serializer<Sink>& s, const std::set<T>& v)
			{
				detail::save_sequence(s, v.begin(), v.end(), v.size(), nullptr);
			}

			template<typename Source, typename T>
			void split_serialize_set(deserializer<Source>& s, std::set<T>& v)
			{
				auto size = s.begin_sequence_load(nullptr);
				while(size--)
				{
					T tmp;
					s >> tmp;
					v.insert(std::move(tmp));
				}
				s.end_sequence_load(nullptr);
			}
		}


		template<typename S, typename T>
		void serialize(S& s, std::set<T>& v)
		{
			s.current_type("std::set");
			detail::split_serialize_set(s, v);
		}
	}
}


#endif