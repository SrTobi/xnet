#pragma once
#ifndef _CHAT_SERVER_HPP
#define _CHAT_SERVER_HPP

#include <xnet/package_socket_adapter.hpp>
#include "chat_service.hpp"

class chat_server
{
	typedef xnet::package_socket_adapter<tcp::socket> socket_type;

	class participant;
	class root_service;
	class chat_room;
	class chat_room_interface;
public:
	chat_server(asio::io_service& ios, xnet::package_factory& pfactory);
	~chat_server();

	void start(const tcp::endpoint& endpoint);
private:
	void do_accept();
	
private:
	asio::io_service& _ios;
	tcp::acceptor _acceptor;
	tcp::socket _destSocket;
	tcp::endpoint _accEndpoint;
	xnet::package_factory& _pfactory;

};



#endif