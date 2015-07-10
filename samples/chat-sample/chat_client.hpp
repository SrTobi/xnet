#pragma once
#ifndef _CHAT_CLIENT_HPP
#define _CHAT_CLIENT_HPP

#include <xnet/package_socket_adapter.hpp>
#include "chat_service.hpp"

class chat_client
{
public:
	chat_client(asio::io_service& ios, xnet::package_factory& factory);
	~chat_client();

	void start(tcp::resolver::iterator endpoint_it);

private:
	void _start_receive();
	void _service_emit(const xnet::package& package);


private:
	asio::io_service& _ios;
	xnet::package_socket_adapter<tcp::socket> _socket;
	xsv::service_peer _peer;
};





#endif