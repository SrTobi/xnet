#include <iostream>
#include <xnet/service/remote_service.hpp>
#include <xnet/service/service_peer.hpp>

using xnet::service::remote_service;

XNET_SERVICE(ChatService)
{
public:
	virtual std::string chat(const std::string& msg) = 0;
};

XNET_IMPLEMENT_SERVICE_DESCRIPTOR(ChatService, desc)
{
	desc.add_method("chat", &ChatService::chat);
}

XNET_SERVICE(LoginService)
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
		if (password != "test")
		{
			throw std::exception("Wrong password");
		}
		return std::make_shared<ChatServiceImpl>();
	}
};

int main()
{
	xnet::package_factory fac;
	using namespace xnet::service;
	service_peer clientPeer(&fac);
	service_peer serverPeer(&fac);

	serverPeer.add_service("loginService", std::make_shared<LoginServiceImpl>());

	remote_service<ChatService> chat_service;

	while (!chat_service)
	{
		std::cout << "Enter password: ";
		std::string pw; std::cin >> pw;
		auto pack = clientPeer.make_call("loginService", &LoginService::login, [&chat_service](remote_service<ChatService>& rem){
			std::cout << "Logged in!" << std::endl;
			chat_service = rem;
		}, [](const xnet::service::call_error& e){
			std::cout << "Error while logging in: " << e.what() << std::endl;
		}, pw);

		pack = serverPeer.process_package(pack);
		clientPeer.process_package(pack);
	}

	while (true)
	{
		std::string msg;
		std::cout << "Enter message for server: ";
		std::cin >> msg;
		auto pack = clientPeer.make_call(chat_service,
									&ChatService::chat,
									[](std::string m) { std::cout << "Message from server: " << m << std::endl; },
									[](const call_error& e){ std::cout << "Error while chatting: " << e.what() << std::endl; },
									msg);

		pack = serverPeer.process_package(pack);
		clientPeer.process_package(pack);
	}
}