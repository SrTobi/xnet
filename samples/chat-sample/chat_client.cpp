#include <iostream>
#include "chat_client.hpp"

chat_client::chat_client(asio::io_service& ios)
	: _ios(ios)
	, _socket(ios, tcp::v4())
{

}

chat_client::~chat_client()
{

}

void chat_client::start(tcp::resolver::iterator endpoint_it)
{
	auto& real_socket = _socket.lowest_layer();
	asio::connect(real_socket, endpoint_it);
}

