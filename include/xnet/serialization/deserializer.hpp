#pragma once
#ifndef _XNET_SERIALIZATION_DESERIALIZER_HPP
#define _XNET_SERIALIZATION_DESERIALIZER_HPP

#include "context.hpp"
#include "serialization.hpp"

namespace xnet {
	namespace serialization {

		template<typename Source, typename Context = context<> >
		class deserializer : public detail::serializer_base<deserializer<Source, Context>>
		{
			static_assert(is_context<Context>::value, "Context must be a context!");
		public:
			deserializer(Source& source)
				: _source(source)
			{
			}

			deserializer(Source& source, const Context& context)
				: _source(source)
				, _context(context)
			{
			}

			template<typename C>
			C& context()
			{
				return _context.get<C>();
			}

			template<typename... Args>
			auto with_context(Args&... contexts)
				-> deserializer<Source, decltype(_context.with(contexts...))>
			{
				auto newContext = _context.with(contexts...);
				return deserializer<Source,  decltype(newContext)>(_source, std::move(newContext));
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
			Context _context;
		};
	}
}


#endif