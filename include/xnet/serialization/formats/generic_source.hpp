#pragma once
#ifndef _XNET_SERIALIZATION_FORMATS_GENERIC_SOURCE_HPP
#define _XNET_SERIALIZATION_FORMATS_GENERIC_SOURCE_HPP

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

			virtual void begin_sequence_load(const char* tag, std::size_t& size) = 0;
			virtual void end_sequence_load(const char* tag) = 0;

			virtual void check_current_type(const char* tag) const = 0;
		};

	}
}


#endif