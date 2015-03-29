#pragma once
#ifndef _XNET_SERIALIZATION_SERIALIZER_HPP
#define _XNET_SERIALIZATION_SERIALIZER_HPP


#include "serialization.hpp"

namespace xnet {
	namespace serialization {

		template<typename Sink>
		class serializer : public detail::serializer_base<serializer<Sink>>
		{
		public:
			serializer(Sink& source)
				: _sink(source)
			{
			}

			void current_type(const char* type)
			{
				_sink.set_current_type(type);
			}

			template<typename T>
			serializer& operator & (const T& out)
			{
				_save(out);
				return *this;
			}

			template<typename T>
			serializer& operator << (const T& out)
			{
				_save(out);
				return *this;
			}
		private:
			template<typename T>
			void _save(const T& out)
			{
				_sink.begin_element(nullptr);
				_serialize(out);
				_sink.end_element(nullptr);
			}

			template<typename T>
			void _save(const detail::tagged_value<T>& taggedVal)
			{
				_save_tagged_value(taggedVal.value(), taggedVal.name());
			}

			template<typename T>
			void _save_tagged_value(const T& out, const char* tag)
			{
				_sink.begin_element(tag);
				_serialize(out);
				_sink.end_element(tag);
			}

			template<typename T>
			void _serialize(const T& out)
			{
				serialize(*this, const_cast<T&>(out));
			}

			XNET_DETAIL_PRIMITIVE_SAVE_OPERATIONS(inline void _save, { _sink.save(out); }, out)
			XNET_DETAIL_PRIMITIVE_SAVE_OPERATIONS(inline void _save_tagged_value, { _sink.save(out, tag); }, out, const char* tag)
		private:
			Sink& _sink;
		};
	}
}


#endif