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
			serializer& operator & (const T& in)
			{
				_save(in);
				return *this;
			}

			template<typename T>
			serializer& operator << (const T& in)
			{
				_save(in);
				return *this;
			}
		private:
			template<typename T>
			void _save(const T& in)
			{
				_sink.begin_element(nullptr);
				serialize(*this, const_cast<T&>(in));
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
				serialize(*this, out);
				_sink.end_element(tag);
			}

			XNET_DETAIL_PRIMITIVE_SAVE_OPERATIONS(inline void _save, { _sink.save(in); }, in)
			XNET_DETAIL_PRIMITIVE_SAVE_OPERATIONS(inline void _save_tagged_value, { _sink.save(out, tag); }, out, const char* tag)
		private:
			Sink& _sink;
		};
	}
}


#endif