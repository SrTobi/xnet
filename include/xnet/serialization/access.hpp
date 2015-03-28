#pragma once
#ifndef _XNET_SERIALIZATION_ACCESS_HPP
#define _XNET_SERIALIZATION_ACCESS_HPP

#include "detail/serializer_base.hpp"

namespace xnet {
	namespace serialization {
		template<typename Sink>
		class serializer;
		template<typename Source>
		class deserializer;


		class access
		{
			template<typename Serializer, typename T>
			friend void serialize(detail::serializer_base<Serializer>& , T&);
			template<typename Sink, typename T>
			friend void save(detail::serializer_base<serializer<Sink>>& s, const T& in);
			template<typename Source, typename T>
			friend void load(detail::serializer_base<deserializer<Source>>& s, T& out);
			template<typename Sink, typename T>
			friend void split_this(const T* in, serializer<Sink>& s);
			template<typename Source, typename T>
			friend void split_this(T* out, deserializer<Source>& s);
		private:

			template<typename Sink, typename T>
			static void _call_serialize(serializer<Sink>& s, const T& in)
			{
				const_cast<T&>(in).serialize(s);
			}

			template<typename Source, typename T>
			static void _call_serialize(deserializer<Source>& s, T& out)
			{
				out.serialize(s);
			}

			template<typename Sink, typename T>
			static void _call_save(serializer<Sink>& s, const T& in)
			{
				in.save(s);
			}

			template<typename Source, typename T>
			static void _call_load(deserializer<Source>& s, T& out)
			{
				out.load(s);
			}
		};
	}
}
#endif