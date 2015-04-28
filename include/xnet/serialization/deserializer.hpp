#pragma once
#ifndef _XNET_SERIALIZATION_DESERIALIZER_HPP
#define _XNET_SERIALIZATION_DESERIALIZER_HPP

#include "serialization.hpp"

namespace xnet {
	namespace serialization {

		template<typename Source, typename Context = std::tuple<> >
		class deserializer : public detail::serializer_base<deserializer<Source, Context>>
		{
		public:
			typedef typename detail::make_ref_tuple<Context>::type context_type;
		public:
			deserializer(Source& source)
				: _source(source)
			{
			}

			deserializer(Source& source, const context_type& context)
				: _source(source)
				, _context(context)
			{
			}

			template<typename C>
			C& context()
			{
				return detail::get_tuple_item_by_type<C&, context_type, 0>::func::get(_context);
			}



			void current_type(const char* type)
			{
				_source.check_current_type(type);
			}

			std::size_t begin_sequence_load(const char* tag)
			{
				std::size_t size = 0;
				_source.begin_sequence_load(tag, size);
				return size;
			}

			void end_sequence_load(const char* tag)
			{
				_source.end_sequence_load(tag);
			}

			template<typename T>
			deserializer& operator & (T& in)
			{
				_load(in);
				return *this;
			}

			template<typename T>
			deserializer& operator >> (T& in)
			{
				_load(in);
				return *this;
			}
		private:
			template<typename T>
			void _load(T& in)
			{
				_source.begin_element(nullptr);
				serialize(*this, in);
				_source.end_element(nullptr);
			}

			template<typename T>
			void _load(detail::tagged_value<T>& taggedVal)
			{
				_load_tagged_value(taggedVal.value(), taggedVal.name());
			}

			template<typename T>
			void _load_tagged_value(T& in, const char* tag)
			{
				_source.begin_element(tag);
				serialize(*this, in);
				_source.end_element(tag);
			}

			XNET_DETAIL_PRIMITIVE_LOAD_OPERATIONS(inline void _load, { _source.load(in); }, in)
			XNET_DETAIL_PRIMITIVE_LOAD_OPERATIONS(inline void _load_tagged_value, { _source.load(in, tag); }, in, const char* tag)

		private:
			Source& _source;
			context_type _context;
		};
	}
}


#endif