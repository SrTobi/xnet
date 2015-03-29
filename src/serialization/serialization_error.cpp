#include "xnet/serialization/serialization_error.hpp"


namespace xnet {
	
	serialization_error::serialization_error(const std::string& what_arg)
		: runtime_error(what_arg)
	{
	}

	serialization_error::serialization_error(const char* what_arg)
		: runtime_error(what_arg)
	{

	}

}