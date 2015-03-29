#pragma once
#ifndef _XNET_SERIALIZATION_SERIALIZATION_HPP
#define _XNET_SERIALIZATION_SERIALIZATION_HPP

#include "access.hpp"
#include "detail/tagged_value.hpp"

namespace xnet {
	namespace serialization {
	
		// The user has not defined any free serialization function
		// so try to call a member function
		template<typename Serializer, typename T>
		void serialize(detail::serializer_base<Serializer>& des, T& v)
		{
			access::_call_serialize(des.inner(), v);
		}

		template<typename T>
		detail::tagged_value<T> tagval(const char* name, T& value)
		{
			return detail::tagged_value<T>(name, value);
		}
	}

	using serialization::tagval;
}

#endif