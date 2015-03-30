#pragma once
#ifndef _XNET_SERIALIZATION_TYPE_PAIR_HPP
#define _XNET_SERIALIZATION_TYPE_PAIR_HPP

#include <utility>
#include "../serialization.hpp"

namespace xnet {
	namespace serialization {
		template<typename S, typename L, typename R>
		void serialize(S& s, std::pair<L, R>& p)
		{
			s.current_type("std::pair");
			s
				& ::xnet::serialization::tagval("first", p.first)
				& ::xnet::serialization::tagval("second", p.second);
		}
	}
}


#endif