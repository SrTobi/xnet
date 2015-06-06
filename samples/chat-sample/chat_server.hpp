#pragma once
#ifndef _CHAT_SERVER_HPP
#define _CHAT_SERVER_HPP

#include <xnet/package_socket_adapter.hpp>
#include "chat_service.hpp"

class chat_server
{
	typedef xnet::package_socket_adapter<tcp::socket> socket_type;
public:
	chat_server(asio::io_service& ios);
	~chat_server();

	void start(const tcp::endpoint& endpoint);
private:
	void do_accept();
	
private:
	asio::io_service& _ios;
	tcp::acceptor _acceptor;
	tcp::socket _socket;
	tcp::endpoint _accEndpoint;
};



#endif