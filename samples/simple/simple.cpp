#include <boost/asio.hpp>
#include <boost/asio/use_future.hpp>
#include <iostream>
#include <future>
#include "xnet/multicast_server.hpp"
#include "xnet/multicast_client.hpp"


namespace asio = boost::asio;
using namespace xnet;




void start_client()
{


	try {
		asio::io_service service;
		//asio::io_service::work work(service);
		multicast_client client(service, "simple");

		client.open(boost::asio::ip::udp::v6());

		//std::cout << "Client: " << client.underlying_socket().local_endpoint() << std::endl;

		std::cout << "Client target: " << client.multicast_endpoint() << std::endl;


		auto result = client.discover();

		/*std::function<void(const boost::system::error_code, const multicast_server::result&)> func = [&](const boost::system::error_code& ec, const multicast_server::result& result)
		{
			//std::cout << result.message() << std::endl;
			//discoverer.async_search(func);
		};*/
		std::cout << "[" << result.origin() << "]: " << result.content().message();
		std::cin.get();
	}
	catch (const std::exception& e)
	{
		std::cout << e.what() << std::endl;
		std::cin.get();
	}
}

void start_server()
{


	try {
		asio::io_service service;
		//asio::io_service::work work(service);
		multicast_server server(service, "simple");

		server.open(boost::asio::ip::udp::v6());

		std::cout << "Server: " << server.underlying_socket().local_endpoint() << std::endl;
		//std::cout << "Client: " << client.underlying_socket().local_endpoint() << std::endl;

		std::cout << "Server target: " << server.multicast_endpoint() << std::endl;

		server.announce(lan_discovery_content("a simple tests"));
		//auto result = client.discover();

		/*std::function<void(const boost::system::error_code, const multicast_server::result&)> func = [&](const boost::system::error_code& ec, const multicast_server::result& result)
		{
			//std::cout << result.message() << std::endl;
			//discoverer.async_search(func);
		};*/
		//std::cout << "[" << result.origin() << "]: " << result.content().message();
		std::cin.get();
	}
	catch (const std::exception& e)
	{
		std::cout << e.what() << std::endl;
		std::cin.get();
	}
}




int main()
{	
	char c;

	while (true)
	{
		std::cout << "Server(s) or client(c): ";
		std::cin >> c;
		std::cin.ignore();
		std::cin.clear();

		if (c == 's')
		{
			start_server();
			return 0;
		}
		else if (c == 'c')
		{
			start_client();
			return 0;
		}
	}

	return 0;
}