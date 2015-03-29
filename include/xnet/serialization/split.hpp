#pragma once
#ifndef _XNET_SERIALIZATION_SPLIT_HPP
#define _XNET_SERIALIZATION_SPLIT_HPP

#include "access.hpp"

namespace xnet {
	namespace serialization {
		template<typename Sink, typename T>
		void split(serializer<Sink>& s, const T& out)
		{
			save(s, out);
		}

		template<typename Source, typename T>
		void split(deserializer<Source>& s, T& in)
		{
			load(s, in);
		}

		template<typename Sink, typename T>
		void split_this(const T* out, serializer<Sink>& s)
		{
			access::_call_save(s, *out);
		}

		template<typename Source, typename T>
		void split_this(T* in, deserializer<Source>& s)
		{
			access::_call_load(s, *in);
		}

		template<typename Sink, typename T>
		void save(detail::serializer_base<serializer<Sink>>& s, const T& out)
		{
			access::_call_save(s.inner(), out);
		}

		template<typename Source, typename T>
		void load(detail::serializer_base<deserializer<Source>>& s, T& in)
		{
			access::_call_load(s.inner(), in);
		}
	}
}


#endif