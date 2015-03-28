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
				serialize(*this, const_cast<T&>(in));
			}

			void _save(const std::string& in)
			{
				_sink.save(in);
			}

			void _save(const int in)
			{
				_sink.save(in);
			}

		private:
			Sink& _sink;
		};
	}
}


#endif