#pragma once
#ifndef _XNET_BASIC_MULTICAST_SERVER_HPP
#define _XNET_BASIC_MULTICAST_SERVER_HPP


#include <chrono>
#include <boost/make_shared.hpp>
#include "multicast_base.hpp"
#include "lan_discovery_content.hpp"

namespace xnet {

	template<typename ContentType>
	class basic_multicast_server : public basic_multicast_base<ContentType>
	{
	public:
		typedef ContentType				content_type;
		typedef boost::asio::ip::udp	protocol_type;
		typedef protocol_type::socket	socket_type;
		typedef protocol_type::endpoint endpoint_type;
	private:
		typedef std::array<char, 2048> recvbuffer_type;
	public:
		basic_multicast_server(boost::asio::io_service& service, const std::string& token)
			: basic_multicast_base(service, token)
			, _identifier(_create_random_identifier())
			, _sendStrand(service)
		{
		}

		~basic_multicast_server()
		{
		}

		void announce(const content_type& content)
		{
			boost::system::error_code ec;
			announce(content, ec);
			boost::asio::detail::throw_error(ec);
		}

		boost::system::error_code announce(const content_type& content, boost::system::error_code& ec)
		{
			auto message = multicast_protocol::build_message(_token, _identifier, multicast_protocol::Command::Announce, content);
			_socket.send_to(boost::asio::buffer(message), _multicastEndpoint, 0, ec);
			return ec;
		}
		
		template<typename AnnounceHandler>
		BOOST_ASIO_INITFN_RESULT_TYPE(AnnounceHandler,
			void(boost::system::error_code))
			async_announce(const content_type& content, BOOST_ASIO_MOVE_ARG(AnnounceHandler) handler)
		{
			boost::asio::detail::async_result_init<
				AnnounceHandler, void(boost::system::error_code)> init(
				BOOST_ASIO_MOVE_CAST(AnnounceHandler)(handler));

			auto innerHandler = std::move(init.handler);
			auto message = boost::make_shared<std::string>(multicast_protocol::build_message(_token, _identifier, multicast_protocol::Command::Announce, content));
			_socket.async_send(boost::asio::buffer(*message),
				[innerHandler, message](const boost::system::error_code& ec, std::size_t sendLen) mutable
			{
				innerHandler(ec);
			});

			return init.result.get();
		}
	private:
		static std::string _create_random_identifier()
		{
			typedef uint64_t idnum_type;
			auto maxIdNum = std::numeric_limits<idnum_type>::max();
			std::uniform_int_distribution<uint64_t> id_distribution(1, maxIdNum);
			auto seed = static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count());
			std::default_random_engine engine(seed);
			auto numId = id_distribution(engine);

			auto idString = std::to_string(numId);
			BOOST_ASSERT(idString.size() <= 20);
			return idString;
		}
	private:
		const std::string _identifier;

		boost::asio::io_service::strand _sendStrand;

		recvbuffer_type _recvBuffer;
		endpoint_type _recvEndpoint;
	};

	typedef basic_multicast_server<lan_discovery_content> multicast_server;
}




#endif