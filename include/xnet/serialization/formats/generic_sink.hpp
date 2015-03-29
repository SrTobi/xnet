#pragma once
#ifndef _XNET_SERIALIZATION_FORMATS_GENERIC_SINK_HPP
#define _XNET_SERIALIZATION_FORMATS_GENERIC_SINK_HPP

#include "../detail/primitive_operations.hpp"

namespace xnet {
	namespace serialization {

		class generic_sink
		{
		public:
			XNET_DETAIL_PRIMITIVE_SAVE_OPERATIONS(virtual void save, = 0, out);
			XNET_DETAIL_PRIMITIVE_SAVE_OPERATIONS(virtual void save, = 0, out, const char* tag);

			virtual void begin_element(const char* tag) = 0;
			virtual void end_element(const char* tag) = 0;

			virtual void begin_array(const char* tag, std::size_t size) = 0;
			virtual void end_array(const char* tag) = 0;

			virtual void set_current_type(const char* type) = 0;
		};

	}
}


#endif