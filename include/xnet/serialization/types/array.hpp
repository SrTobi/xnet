#pragma once
#ifndef _XNET_SERIALIZATION_TYPE_ARRAY_HPP
#define _XNET_SERIALIZATION_TYPE_ARRAY_HPP

#include <array>
#include "../serialization.hpp"
#include "detail/serialize_sequence.hpp"

namespace xnet {
	namespace serialization {
		template<typename S, typename T, std::size_t Size>
		void serialize(S& s, std::array<T, Size>& v)
		{
			s.current_type("std::array");
			for (auto& e : v)
			{
				s & e;
			}
		}
	}
}


#endif