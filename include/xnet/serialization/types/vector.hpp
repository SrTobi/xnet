#pragma once
#ifndef _XNET_SERIALIZATION_TYPE_VECTOR_HPP
#define _XNET_SERIALIZATION_TYPE_VECTOR_HPP

#include <vector>
#include "../serialization.hpp"
#include "detail/serialize_sequence.hpp"

namespace xnet {
	namespace serialization {
		namespace detail {
			template<typename Sink, typename T>
			void split_serialize_vector(serializer<Sink>& s, const std::vector<T>& v)
			{
				detail::save_sequence(s, v.begin(), v.end(), v.size(), nullptr);
			}

			template<typename Source, typename T>
			void split_serialize_vector(deserializer<Source>& s, std::vector<T>& v)
			{
				auto size = s.begin_sequence_load(nullptr);
				v.resize(size);
				for(auto& e : v)
				{
					s >> e;
				}
				s.end_sequence_load(nullptr);
			}
		}


		template<typename S, typename T>
		void serialize(S& s, std::vector<T>& v)
		{
			s.current_type("std::vector");
			detail::split_serialize_vector(s, v);
		}
	}
}


#endif