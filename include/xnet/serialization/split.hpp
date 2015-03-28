#pragma once
#ifndef _XNET_SERIALIZATION_SPLIT_HPP
#define _XNET_SERIALIZATION_SPLIT_HPP

#include "access.hpp"

namespace xnet {
	namespace serialization {
		template<typename Sink, typename T>
		void split(serializer<Sink>& s, const T& in)
		{
			save(s, in);
		}

		template<typename Source, typename T>
		void split(deserializer<Source>& s, T& out)
		{
			load(s, out);
		}

		template<typename Sink, typename T>
		void split_this(const T* in, serializer<Sink>& s)
		{
			access::_call_save(s, *in);
		}

		template<typename Source, typename T>
		void split_this(T* out, deserializer<Source>& s)
		{
			access::_call_load(s, *out);
		}

		template<typename Sink, typename T>
		void save(detail::serializer_base<serializer<Sink>>& s, const T& in)
		{
			access::_call_save(s.inner(), in);
		}

		template<typename Source, typename T>
		void load(detail::serializer_base<deserializer<Source>>& s, T& out)
		{
			access::_call_load(s.inner(), out);
		}
	}
}


#endif