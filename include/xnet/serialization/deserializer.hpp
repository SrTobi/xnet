#pragma once
#ifndef _XNET_SERIALIZATION_DESERIALIZER_HPP
#define _XNET_SERIALIZATION_DESERIALIZER_HPP

#include "detail/primitive_operations.hpp"
#include "serialization.hpp"

namespace xnet {
	namespace serialization {

		template<typename Source>
		class deserializer : public detail::serializer_base<deserializer<Source>>
		{
		public:
			deserializer(Source& source)
				: _source(source)
			{
			}

			template<typename T>
			deserializer& operator & (T& out)
			{
				_load(out);
				return *this;
			}

			template<typename T>
			deserializer& operator >> (T& out)
			{
				_load(out);
				return *this;
			}
		private:
			template<typename T>
			void _load(T& out)
			{
				_source.begin_element(nullptr);
				serialize(*this, out);
				_source.end_element(nullptr);
			}

			template<typename T>
			void _load(detail::tagged_value<T>& taggedVal)
			{
				_load_tagged_value(taggedVal.value(), taggedVal.name());
			}

			template<typename T>
			void _load_tagged_value(T& out, const char* tag)
			{
				_source.begin_element(tag);
				serialize(*this, out);
				_source.end_element(tag);
			}

			XNET_DETAIL_PRIMITIVE_LOAD_OPERATIONS(inline void _load, { _source.load(out); }, out)
			XNET_DETAIL_PRIMITIVE_LOAD_OPERATIONS(inline void _load_tagged_value, { _source.load(out, tag); }, out, const char* tag)

		private:
			Source& _source;
		};
	}
}


#endif