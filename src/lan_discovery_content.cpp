#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include "xnet/lan_discovery_content.hpp"


namespace xnet {



	lan_discovery_content::lan_discovery_content()
	{
	}

	lan_discovery_content::lan_discovery_content(const std::string& msg)
		: _message(msg)
	{
	}

	lan_discovery_content::~lan_discovery_content()
	{

	}

	std::string lan_discovery_content::serialize() const
	{
		return _message;
	}

}