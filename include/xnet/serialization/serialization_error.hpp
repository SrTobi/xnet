#pragma once
#ifndef _XNET_SERIALIZATION_ERROR_HPP
#define _XNET_SERIALIZATION_ERROR_HPP

#include <stdexcept>

namespace xnet {

	class serialization_error: public std::runtime_error
	{
	public:
		explicit serialization_error(const std::string& what_arg);
		explicit serialization_error(const char* what_arg);
	};


}


#endif