#pragma once
#ifndef _XNET_SERIALIZATION_FORMATS_XML_SOURCE_HPP
#define _XNET_SERIALIZATION_FORMATS_XML_SOURCE_HPP

#include <istream>

namespace xnet {
	namespace serialization {

		template<typename Ch>
		class text_source
		{
		public:
			text_source(std::istream& stream)
				: _stream(stream)
			{
			}

			template<typename T>
			void load(T& out)
			{
				_stream >> out;
			}

			template<typename T>
			void load(T& out, const char* tag)
			{
				load(out);
			}

			void begin_element(const char*)
			{
			}

			void end_element(const char*)
			{
			}

		private:
			std::istream& _stream;
		};

	}
}


#endif