#pragma once
#ifndef _XNET_SERIALIZATION_TYPE_SET_HPP
#define _XNET_SERIALIZATION_TYPE_SET_HPP


#include <set>
#include "../serialization.hpp"
#include "detail/serialize_sequence.hpp"

namespace xnet {
	namespace serialization {
		namespace detail {
			template<typename Sink, typename Context, typename T, typename Comp, typename Alloc>
			void split_serialize_set(serializer<Sink, Context>& s, const std::set<T, Comp, Alloc>& v)
			{
				detail::save_sequence(s, v.begin(), v.end(), v.size(), nullptr);
			}

			template<typename Source, typename Context, typename T, typename Comp, typename Alloc>
			void split_serialize_set(deserializer<Source, Context>& s, std::set<T, Comp, Alloc>& v)
			{
				auto size = s.begin_sequence_load(nullptr);
				v.clear();
				T tmp;
				while(size--)
				{
					s >> tmp;
					v.insert(std::move(tmp));
				}
				s.end_sequence_load(nullptr);
			}
		}


		template<typename S, typename T, typename Comp, typename Alloc>
		void serialize(S& s, std::set<T, Comp, Alloc>& v)
		{
			s.current_type("std::set");
			detail::split_serialize_set(s, v);
		}
	}
}


#endif