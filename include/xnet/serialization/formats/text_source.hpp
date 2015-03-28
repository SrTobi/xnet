#pragma once
#ifndef _XNET_SERIALIZATION_FORMATS_TEXT_SOURCE_HPP
#define _XNET_SERIALIZATION_FORMATS_TEXT_SOURCE_HPP

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

		private:
			std::istream& _stream;
		};

	}
}


#endif