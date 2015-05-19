#include <iostream>
#include <boost/asio/io_service.hpp>
#include <xnet/service/remote_service.hpp>
#include <xnet/service/service_peer.hpp>

using xnet::service::remote_service;

XNET_SERVICE(ChatService)
{
public:
	virtual std::string chat(const std::string& msg) = 0;
};

XNET_IMPLEMENT_SERVICE_DESCRIPTOR(,ChatService, desc)
{
	desc.add_method("chat", &ChatService::chat);
}

XNET_SERVICE(LoginService)
{
public:
	virtual remote_service<ChatService> login(std::string password) = 0;
};

XNET_IMPLEMENT_SERVICE_DESCRIPTOR(,LoginService, desc)
{
	desc.add_method("login", &LoginService::login);
}


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
		clientPeer.call("loginService", &LoginService::login, [&chat_service](remote_service<ChatService>& rem){
			std::cout << "Logged in!" << std::endl;
			chat_service = rem;
		}, [](const xnet::service::call_error& e){
			std::cout << "Error while logging in: " << e.what() << std::endl;
		}, pw);

		io_service.run();
		io_service.reset();
	}

	while (true)
	{
		std::string msg;
		std::cout << "Enter message for server: ";
		std::getline(std::cin, msg);
		clientPeer.call(chat_service,
						&ChatService::chat,
						[](std::string m) { std::cout << "Message from server: " << m << std::endl; },
						[](const call_error& e){ std::cout << "Error while chatting: " << e.what() << std::endl; },
						msg);

		io_service.run();
		io_service.reset();
	}
}