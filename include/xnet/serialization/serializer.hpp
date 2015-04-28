#pragma once
#ifndef _XNET_SERIALIZATION_SERIALIZER_HPP
#define _XNET_SERIALIZATION_SERIALIZER_HPP


#include "serialization.hpp"

namespace xnet {
	namespace serialization {

		template<typename Sink, typename Context = std::tuple<>>
		class serializer : public detail::serializer_base<serializer<Sink, Context>>
		{
		public:
			typedef typename detail::make_ref_tuple<Context>::type context_type;
		public:
			serializer(Sink& sink)
				: _sink(sink)
			{
			}

			serializer(Sink& sink, const context_type& context)
				: _sink(sink)
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
				_sink.set_current_type(type);
			}

			void begin_sequence_save(const char* tag, std::size_t size)
			{
				_sink.begin_sequence_save(tag, size);
			}

			void end_sequence_save(const char* tag)
			{
				_sink.end_sequence_save(tag);
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
			context_type _context;
		};
	}
}


#endif