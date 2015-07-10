#include <iostream>
#include "chat_client.hpp"

chat_client::chat_client(asio::io_service& ios, xnet::package_factory& factory)
	: _ios(ios)
	, _socket(factory, ios, tcp::v4())
	, _peer(&factory, std::bind(&chat_client::_service_emit, this, std::placeholders::_1))
{

}

chat_client::~chat_client()
{

}

void chat_client::start(tcp::resolver::iterator endpoint_it)
{
	auto& real_socket = _socket.lowest_layer();
	asio::connect(real_socket, endpoint_it);
	_start_receive();	

	auto fp = &chat::root_service::chat_rooms;
	_peer.call("root", fp, 
		[this](const std::vector<std::string>& rooms)
		{
			std::cout << "Active rooms:" << std::endl;
			for (auto& room : rooms)
			{
				std::cout << room << std::endl;
			}
		},
		[this](...)
		{
			std::cout << "Can not list rooms..." << std::endl;
		});
}

void chat_client::_start_receive()
{
	_socket.async_receive(
		[this](boost::system::error_code ec, const xnet::package& p)
		{
			if (!ec)
			{
				_peer.process_package(p);
				_start_receive();
			}
			else{
				std::cout << "Disconnected from server [" << ec.message() << "]" << std::endl;
			}
		});
}

void chat_client::_service_emit(const xnet::package& package)
{
	_socket.async_send(package,
		[this](boost::system::error_code ec, std::size_t)
		{
			if (ec)
			{
				std::cout << "Error while sending: " << ec.message() << std::endl;
			}
		});
}

