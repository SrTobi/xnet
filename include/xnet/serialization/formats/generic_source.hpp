#pragma once
#ifndef _XNET_SERIALIZATION_FORMATS_GENERIC_SOURCE_HPP
#define _XNET_SERIALIZATION_FORMATS_GENERIC_SOURCE_HPP

#include <istream>
#include <cstddef>
#include "../detail/primitive_operations.hpp"

namespace xnet {
	namespace serialization {

		class generic_source
		{
		public:
			XNET_DETAIL_PRIMITIVE_LOAD_OPERATIONS(virtual void load, = 0, in);
			XNET_DETAIL_PRIMITIVE_LOAD_OPERATIONS(virtual void load, = 0, in, const char* tag);

			virtual void begin_element(const char* tag) = 0;
			virtual void end_element(const char* tag) = 0;

			virtual bool begin_array(const char* tag, std::size_t& size) = 0;
			virtual void end_array(const char* tag) = 0;
		};

	}
}


#endif