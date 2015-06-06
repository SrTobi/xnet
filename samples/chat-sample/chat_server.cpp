#include <iostream>
#include "chat_server.hpp"


chat_server::chat_server(asio::io_service& ios)
	: _ios(ios)
	, _socket(ios)
	, _acceptor(ios, tcp::v4())
{
}

chat_server::~chat_server()
{

}

void chat_server::start(const tcp::endpoint& endpoint)
{
	boost::system::error_code ec;
	_acceptor.bind(endpoint);
	_acceptor.listen();
	do_accept();
}

void chat_server::do_accept()
{
	_acceptor.async_accept(_socket,
		_accEndpoint,
		[this](boost::system::error_code ec)
		{
			if (!ec)
			{
				std::cout << "New client connected from " << _accEndpoint << "!" << std::endl;
				_socket.close();
			}
			else
			{
				std::cout << "Failed to accept [" << ec.message() << "]!" << std::endl;
			}

			do_accept();
		});
}
