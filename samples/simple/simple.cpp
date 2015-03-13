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


		client.scan();

		std::function<void(const boost::system::error_code&, const multicast_client::result&)> func = [&](const boost::system::error_code& ec, const multicast_client::result& result)
		{
			std::cout << "Message: " << result.content().message() << std::endl;
			client.async_discover(func);
		};
		client.async_discover(func);

		service.run();
	}
	catch (const std::exception& e)
	{
		std::cout << e.what() << std::endl;
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

		lan_discovery_content ldc("a simple tests");
		server.announce(ldc);
		//auto result = client.discover();

		multicast_server::endpoint origin;
		std::function<void(const boost::system::error_code&)> func = [&](const boost::system::error_code& ec)
		{
			if (ec)
			{
				std::cout << ec << std::endl;
				return;
			}

			std::cout << "Scan from " << origin << "!" << std::endl;
				
			server.announce(ldc);
			server.async_capture(origin, func);
		};

		server.async_capture(origin, func);

		service.run();
		//std::cout << "[" << result.origin() << "]: " << result.content().message();
	}
	catch (const std::exception& e)
	{
		std::cout << e.what() << std::endl;
	}
}

void start_both()
{
	try {
		asio::io_service service;
		//asio::io_service::work work(service);
		multicast_server server(service, "simple");
		multicast_client client(service, "simple");

		server.open(boost::asio::ip::udp::v4());
		client.open(boost::asio::ip::udp::v4());

		std::cout << "Server: " << server.underlying_socket().local_endpoint() << std::endl;
		std::cout << "Client: " << client.underlying_socket().local_endpoint() << std::endl;

		std::cout << "Server target: " << server.multicast_endpoint() << std::endl;


		std::function<void(const boost::system::error_code&, const multicast_client::result&)> func = [&](const boost::system::error_code& ec, const multicast_client::result& result)
		{
			std::cout << result.content().message() << std::endl;
			client.async_discover(func);
		};
		client.async_discover(func);

		server.async_announce(lan_discovery_content("a simple tests"), asio::use_future);
		service.run();
	}
	catch (const std::exception& e)
	{
		std::cout << e.what() << std::endl;
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
			break;
		}
		else if (c == 'c')
		{
			start_client();
			break;
		}
		else if (c == 'b')
		{
			start_both();
			break;
		}
	}
	std::cin.get();

	return 0;
}