#include <iostream>
#include <map>
#include <set>
#include "chat_server.hpp"


/************************************** chat_server::chat_room_interface definition **************************************/
class chat_server::chat_room_interface
	: public chat::chat_room
{
public:
	chat_room_interface(const std::shared_ptr<chat_server::chat_room>& room, const std::string& name, xsv::remote_service<chat::chat_room_subscription> subscription)
		: _room(room)
		, _name(name)
		, _subscription(subscription)
	{
	}

	~chat_room_interface();
	virtual void post_message(const std::string& msg);

	void send_message(const std::string& user, const std::string& message);
	void send_user_joined(const std::string& user);
	void send_user_left(const std::string& user);

	const std::string& name() const;
private:
	std::string _name;
	xsv::remote_service<chat::chat_room_subscription> _subscription;
	std::shared_ptr<chat_server::chat_room> _room;
};

/************************************** chat_server::chat_room implementation **************************************/
class chat_server::chat_room
{
public:

	void add_user(const std::shared_ptr<chat_room_interface>& user)
	{
		for (auto& p : _users)
		{
			p.second.lock()->send_user_joined(user->name());
		}
		_users.emplace(user->name(),  user);
	}

	void remove_user(const std::string& name)
	{
		_users.erase(name);
		for (auto& p : _users)
		{
			p.second.lock()->send_user_left(name);
		}
	}

	void send_message(const std::string& from, const std::string& msg)
	{
		for (auto& p : _users)
		{
			p.second.lock()->send_message(from, msg);
		}
	}

private:
	std::map<std::string, std::weak_ptr<chat_room_interface>> _users;
};

/************************************** chat_server::chat_room_interface implementation **************************************/
chat_server::chat_room_interface::~chat_room_interface()
{
	_room->remove_user(_name);
}

void chat_server::chat_room_interface::post_message(const std::string& msg)
{
	_room->send_message(_name, msg);
}

void chat_server::chat_room_interface::send_message(const std::string& user, const std::string& msg)
{
	_subscription.invoke(&chat::chat_room_subscription::new_message, user, msg);
}

const std::string& chat_server::chat_room_interface::name() const
{
	return _name;
}

void chat_server::chat_room_interface::send_user_joined(const std::string& user)
{
	_subscription.invoke(&chat::chat_room_subscription::user_joined, user);
}

void chat_server::chat_room_interface::send_user_left(const std::string& user)
{
	_subscription.invoke(&chat::chat_room_subscription::user_left, user);
}

/************************************** chat_server::root_service implementation **************************************/
class chat_server::root_service
	: public chat::root_service
{
public:
	root_service()
	{
	}

	virtual std::vector<std::string> chat_rooms() override
	{
		std::vector<std::string> result;
		for (auto& p : _rooms)
			result.push_back(p.first);
		return result;
	}

	virtual xsv::remote_service<chat::chat_room> enter_room(const std::string& name, xsv::remote_service<chat::chat_room_subscription> subscription)
	{
		auto it = _rooms.find(name);
		if (it == _rooms.end())
		{
			std::tie(it, std::ignore)
				= _rooms.emplace(name, std::make_shared<chat_room>());
		}
		
		auto room = it->second;
		auto user = std::make_shared<chat_room_interface>(room, name, std::move(subscription));

		room->add_user(user);

		return user;
	}

private:
	std::map<std::string, std::shared_ptr<chat_room>> _rooms;
};

class chat_server::participant
	: public std::enable_shared_from_this<participant>
{
public:
	participant(tcp::socket&& s, xnet::package_factory& pfactory)
		: _socket(pfactory, std::move(s))
		, _peer(&pfactory, std::bind(&participant::_service_emit, this, std::placeholders::_1))
	{
	}

	void start()
	{
		_start_receive();
	}

private:
	void _start_receive()
	{
		auto self = shared_from_this();
		_socket.async_receive(
			[this, self](const boost::system::error_code& ec, const xnet::package& p)
			{
				if (!ec)
				{
					_peer.process_package(p);
				}
				else{
					std::cout << "Participant disconnected [" << ec.message() << "]" << std::endl;
				}

			});
	}

	void _service_emit(const xnet::package& package)
	{
		_socket.async_send(package,
			[this](boost::system::error_code ec, std::size_t)
			{
				if (ec)
				{
					std::cout << "Error while sending: " << ec.message() << std::endl;
				}
			});
	}
private:
	xnet::package_socket_adapter<tcp::socket> _socket;
	xsv::service_peer _peer;
};



chat_server::chat_server(asio::io_service& ios, xnet::package_factory& pfactory)
	: _ios(ios)
	, _destSocket(ios)
	, _acceptor(ios, tcp::v4())
	, _pfactory(pfactory)
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
	_acceptor.async_accept(_destSocket,
		_accEndpoint,
		[this](boost::system::error_code ec)
		{
			if (!ec)
			{
				std::cout << "New client connected from " << _accEndpoint << "!" << std::endl;
				auto p = std::make_shared<participant>(std::move(_destSocket), _pfactory);
				p->start();
			}
			else
			{
				std::cout << "Failed to accept [" << ec.message() << "]!" << std::endl;
			}

			do_accept();
		});
}

