#pragma once
#ifndef _XNET_SERIALIZATION_SPLIT_HPP
#define _XNET_SERIALIZATION_SPLIT_HPP

#include "access.hpp"

namespace xnet {
	namespace serialization {
		template<typename Sink, typename Context, typename T>
		void split(serializer<Sink, Context>& s, const T& out)
		{
			save(s, out);
		}

		template<typename Source, typename Context, typename T>
		void split(deserializer<Source, Context>& s, T& in)
		{
			load(s, in);
		}

		template<typename Sink, typename Context, typename T>
		void split_this(const T* out, serializer<Sink, Context>& s)
		{
			access::_call_save(s, *out);
		}

		template<typename Source, typename Context, typename T>
		void split_this(T* in, deserializer<Source, Context>& s)
		{
			access::_call_load(s, *in);
		}

		template<typename Sink, typename Context, typename T>
		void save(detail::serializer_base<serializer<Sink, Context>>& s, const T& out)
		{
			access::_call_save(s.inner(), out);
		}

		template<typename Source, typename Context, typename T>
		void load(detail::serializer_base<deserializer<Source, Context>>& s, T& in)
		{
			access::_call_load(s.inner(), in);
		}
	}
}


#endif