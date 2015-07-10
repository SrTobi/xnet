#include <iostream>
#include <string>
#include "chat_server.hpp"
#include "chat_client.hpp"

const unsigned short PORT = 31238;

void do_client(unsigned short port)
{
	asio::io_service ios;
	std::string addr;
	xnet::package_factory pfactory;

	std::cout << "Target address: ";
	std::getline(std::cin, addr);

	
	//try
	{
		std::cout << "Connecting to " << addr << ":" << port << "..." << std::endl;
		chat_client client(ios, pfactory);

		tcp::resolver resolver(ios);
		auto it = resolver.resolve({ addr, std::to_string(port) });
		client.start(it);

		std::cout << "Successfully connected to chat server!" << std::endl;

		ios.run();
	}
	//catch (std::exception& e)
	{
		//std::cout << "Failed to start chat server [" << e.what() << "]!" << std::endl;
	}
}


void do_server(unsigned short port)
{
	asio::io_service ios;
	xnet::package_factory pfactory;
	std::cout << "Start chat server on port " << port << "..." << std::endl;
	
	try
	{
		chat_server server(ios, pfactory);

		tcp::endpoint server_bind(tcp::v4(), port);
		server.start(server_bind);

		std::cout << "Started chat server!" << std::endl;

		ios.run();
	}
	catch (std::exception& e)
	{
		std::cout << "Failed to start chat server [" << e.what() << "]!" << std::endl;
	}
}

int main(int argc, const char** argv)
{
	unsigned short port = PORT;
	if (argc > 1)
	{
		port = std::atoi(argv[1]);
	}

	std::string input;

	while (true)
	{
		std::cout << "Server or client: ";
		std::getline(std::cin, input);

		if (!input.empty())
		{
			switch (input.front())
			{
			case 's':
				do_server(port);
				return 0;

			case 'c':
				do_client(port);
				return 0;

			default:
				break;
			}
		}
	}


	return 0;
}