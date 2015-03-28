#pragma once
#ifndef _XNET_SERIALIZATION_FORMATS_TEXT_SINK_HPP
#define _XNET_SERIALIZATION_FORMATS_TEXT_SINK_HPP

#include <istream>

namespace xnet {
	namespace serialization {

		template<typename Ch>
		class text_sink
		{
		public:
			text_sink(std::ostream& stream)
				: _stream(stream)
			{
			}

			template<typename T>
			void save(const T& in)
			{
				_stream << in << ' ';
			}

		private:
			std::ostream& _stream;
		};

	}
}


#endif