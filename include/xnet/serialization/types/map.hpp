#pragma once
#ifndef _XNET_SERIALIZATION_TYPE_MAP_HPP
#define _XNET_SERIALIZATION_TYPE_MAP_HPP


#include <map>
#include "pair.hpp"
#include "../serialization.hpp"
#include "detail/serialize_sequence.hpp"

namespace xnet {
	namespace serialization {
		namespace detail {
			template<typename Sink, typename Context, typename K, typename T, typename Comp, typename Alloc>
			void split_serialize_map(serializer<Sink, Context>& s, const std::map<K, T, Comp, Alloc>& v)
			{
				detail::save_sequence(s, v.begin(), v.end(), v.size(), nullptr);
			}

			template<typename Source, typename Context, typename K, typename T, typename Comp, typename Alloc>
			void split_serialize_map(deserializer<Source, Context>& s, std::map<K, T, Comp, Alloc>& v)
			{
				auto size = s.begin_sequence_load(nullptr);
				v.clear();
				std::pair<K, T> tmp;
				while(size--)
				{
					s >> tmp;
					v.insert(std::move(tmp));
				}
				s.end_sequence_load(nullptr);
			}
		}


		template<typename S, typename K, typename T, typename Comp, typename Alloc>
		void serialize(S& s, std::map<K, T, Comp, Alloc>& v)
		{
			s.current_type("std::map");
			detail::split_serialize_map(s, v);
		}
	}
}


#endif