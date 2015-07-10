#pragma once
#ifndef _CHAT_SERVICE_HPP
#define _CHAT_SERVICE_HPP

#include <boost/asio.hpp>
#include <xnet/package_socket_adapter.hpp>
#include <xnet/service/generic_service.hpp>
#include <xnet/service/service_peer.hpp>
#include <xnet/service/remote_service.hpp>
#include <xnet/serialization/types/vector.hpp>

namespace asio = boost::asio;
using tcp = boost::asio::ip::tcp;
namespace xsv = xnet::service;

namespace chat
{
	XNET_SERVICE(chat_room_subscription)
	{
		virtual void user_joined(const std::string& name) = 0;
		virtual void user_left(const std::string& name) = 0;
		virtual void new_message(const std::string& username, const std::string& msg) = 0;
	};

	XNET_SERVICE(chat_room)
	{
		virtual void post_message(const std::string& msg) = 0;
	};

	XNET_SERVICE(root_service)
	{
		virtual std::vector<std::string> chat_rooms() = 0;
		virtual xsv::remote_service<chat_room> enter_room(const std::string& name, xsv::remote_service<chat_room_subscription> subscription) = 0;
	};

}






#endif