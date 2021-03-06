#pragma once
#ifndef _XNET_SERIALIZATION_TYPE_UNORDEREDMAP_HPP
#define _XNET_SERIALIZATION_TYPE_UNORDEREDMAP_HPP


#include <unordered_map>
#include "pair.hpp"
#include "../serialization.hpp"
#include "detail/serialize_sequence.hpp"

namespace xnet {
	namespace serialization {
		namespace detail {
			template<typename Sink, typename K, typename T, typename Hasher, typename Comp, typename Alloc>
			void split_serialize_unordered_map(serializer<Sink>& s, const std::unordered_map<K, T, Hasher, Comp, Alloc>& v)
			{
				detail::save_sequence(s, v.begin(), v.end(), v.size(), nullptr);
			}

			template<typename Source, typename K, typename T, typename Hasher, typename Comp, typename Alloc>
			void split_serialize_unordered_map(deserializer<Source>& s, std::unordered_map<K, T, Hasher, Comp, Alloc>& v)
			{
				auto size = s.begin_sequence_load(nullptr);
				v.clear();
				std::pair<K, T> tmp;
				while (size--)
				{
					s >> tmp;
					v.insert(std::move(tmp));
				}
				s.end_sequence_load(nullptr);
			}
		}


		template<typename S, typename K, typename T, typename Hasher, typename Comp, typename Alloc>
		void serialize(S& s, std::unordered_map<K, T, Hasher, Comp, Alloc>& v)
		{
			s.current_type("std::unordered_map");
			detail::split_serialize_unordered_map(s, v);
		}
	}
}


#endif