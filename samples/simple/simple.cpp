#include <boost/asio.hpp>
#include <boost/asio/use_future.hpp>
#include <iostream>
#include "xnet/lan_discoverer.hpp"


namespace asio = boost::asio;
using namespace xnet;

int main()
{	
	try {
		asio::io_service service;
		asio::io_service::work work(service);
		lan_discoverer discoverer(service, "simple");

		discoverer.open(lan_discoverer::protocol_type::v4(), 12345);
		discoverer.scan(12345);
		discoverer.announce(12345, lan_discovery_content("a simple tests"));

		std::function<void(const boost::system::error_code, const lan_discoverer::result&)> func = [&](const boost::system::error_code& ec, const lan_discoverer::result& result)
		{
			//std::cout << result.message() << std::endl;
			//discoverer.async_search(func);
		};
		discoverer.discover();


		service.run();
	}
	catch (const std::exception& e)
	{
		std::cout << e.what() << std::endl;
	}
	std::cin.get();

	return 0;
}