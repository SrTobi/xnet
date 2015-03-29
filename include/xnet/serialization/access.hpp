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
			friend void save(detail::serializer_base<serializer<Sink>>& s, const T& out);
			template<typename Source, typename T>
			friend void load(detail::serializer_base<deserializer<Source>>& s, T& in);
			template<typename Sink, typename T>
			friend void split_this(const T* out, serializer<Sink>& s);
			template<typename Source, typename T>
			friend void split_this(T* in, deserializer<Source>& s);
		private:

			template<typename Sink, typename T>
			static void _call_serialize(serializer<Sink>& s, const T& out)
			{
				const_cast<T&>(out).serialize(s);
			}

			template<typename Source, typename T>
			static void _call_serialize(deserializer<Source>& s, T& in)
			{
				in.serialize(s);
			}

			template<typename Sink, typename T>
			static void _call_save(serializer<Sink>& s, const T& out)
			{
				out.save(s);
			}

			template<typename Source, typename T>
			static void _call_load(deserializer<Source>& s, T& in)
			{
				in.load(s);
			}
		};
	}
}
#endif