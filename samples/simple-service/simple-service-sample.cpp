#include <iostream>
#include <xnet/service/remote_service.hpp>
#include <xnet/service/service_peer.hpp>

using xnet::service::remote_service;

class ChatService
{
public:
	virtual std::string chat(const std::string& msg) = 0;
};

XNET_IMPLEMENT_SERVICE_DESCRIPTOR(ChatService, desc)
{
	desc.add_method("chat", &ChatService::chat);
}

class LoginService
{
public:
	virtual remote_service<ChatService> login(std::string password) = 0;
};

XNET_IMPLEMENT_SERVICE_DESCRIPTOR(LoginService, desc)
{
	desc.add_method("login", &LoginService::login);
}


class ChatServiceImpl : public ChatService
{
public:
	virtual std::string chat(const std::string& msg) override
	{
		std::cout << "From client: " << msg << std::endl << "Answer: " << std::endl;
		std::string answ;
		std::cin >> answ;
		return answ;
	}
};

class LoginServiceImpl : public LoginService
{
public:
	virtual remote_service<ChatService> login(std::string password) override
	{
		return std::make_shared<ChatServiceImpl>();
	}
};

int main()
{
	using namespace xnet::service;
	service_peer clientPeer;
	service_peer serverPeer;

	serverPeer.add_service("loginService", std::make_shared<LoginServiceImpl>());
}