#include <iostream>
#include <boost/asio/io_service.hpp>

#include "service-definition.hpp"
#include "RemoteChatService.hpp"
#include "RemoteLoginService.hpp"


class ChatServiceImpl : public ChatService
{
public:
	virtual std::string chat(const std::string& msg) override
	{
		std::cout << "From client: " << msg << std::endl << "Answer: ";
		std::string answ;
		std::getline(std::cin, answ);
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
	using namespace xnet::service;

	std::function<void(const xnet::package&)> to_client;
	std::function<void(const xnet::package&)> to_server;
	boost::asio::io_service io_service;
	xnet::package_factory fac;
	auto post_to_server = [&](const xnet::package& p) { io_service.post(std::bind(to_server, p)); };
	auto post_to_client = [&](const xnet::package& p) { io_service.post(std::bind(to_client, p)); };
	service_peer clientPeer(&fac, post_to_server);
	service_peer serverPeer(&fac, post_to_client);

	to_client = [&](const xnet::package& p)
	{
		clientPeer.process_package(p);
	};

	to_server = [&](const xnet::package& p)
	{
		serverPeer.process_package(p);
	};

	serverPeer.add_service("loginService", std::make_shared<LoginServiceImpl>());

	remote_service<ChatService> chat_service;

	while (!chat_service)
	{
		std::cout << "Enter password: ";
		std::string pw;
		std::getline(std::cin, pw);
		clientPeer.static_service<LoginService>("loginService")->login(pw, [&chat_service](remote_service<ChatService> rem){
			std::cout << "Logged in!" << std::endl;
			chat_service = rem;
		}, [](const xnet::service::call_error& e){
			std::cout << "Error while logging in: " << e.what() << std::endl;
		});

		io_service.run();
		io_service.reset();
	}

	while (true)
	{
		std::string msg;
		std::cout << "Enter message for server: ";
		std::getline(std::cin, msg);
		chat_service->chat(
						msg,
						[](std::string m) { std::cout << "Message from server: " << m << std::endl; },
						[](const call_error& e){ std::cout << "Error while chatting: " << e.what() << std::endl; }
						);

		io_service.run();
		io_service.reset();
	}
}