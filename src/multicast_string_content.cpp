#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include "xnet/multicast_string_content.hpp"


namespace xnet {



	multicast_string_content::multicast_string_content()
	{
	}

	multicast_string_content::multicast_string_content(const std::string& msg)
		: _message(msg)
	{
	}

	multicast_string_content::~multicast_string_content()
	{

	}

	std::string multicast_string_content::serialize() const
	{
		return _message;
	}

}