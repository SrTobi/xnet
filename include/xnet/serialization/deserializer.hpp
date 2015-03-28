#pragma once
#ifndef _XNET_SERIALIZATION_DESERIALIZER_HPP
#define _XNET_SERIALIZATION_DESERIALIZER_HPP


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
				serialize(*this, out);
			}

			void _load(std::string& out)
			{
				_source.load(out);
			}

			void _load(int& out)
			{
				_source.load(out);
			}

		private:
			Source& _source;
		};
	}
}


#endif