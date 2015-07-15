#pragma once
#ifndef _SERVICE_DEFINITION_HPP
#define _SERVICE_DEFINITION_HPP

#include <xnet/service/remote_service.hpp>
#include <xnet/service/service_peer.hpp>


using xnet::service::remote_service;

XNET_SERVICE(ChatService)
{
public:
	virtual std::string chat(const std::string& msg) = 0;
};

/*XNET_IMPLEMENT_SERVICE_DESCRIPTOR(,ChatService, desc)
{
	desc.add_method("chat", &ChatService::chat);
}*/

XNET_SERVICE(LoginService)
{
public:
	virtual remote_service<ChatService> login(std::string password) = 0;
};

/*XNET_IMPLEMENT_SERVICE_DESCRIPTOR(,LoginService, desc)
{
	desc.add_method("login", &LoginService::login);
}*/

#endif