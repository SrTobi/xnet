#pragma once
#ifndef _XNET_BASIC_MULTICAST_SERVER_HPP
#define _XNET_BASIC_MULTICAST_SERVER_HPP


#include <chrono>
#include <boost/make_shared.hpp>
#include "multicast_base.hpp"
#include "multicast_string_content.hpp"

namespace xnet {

	template<typename ContentType>
	class basic_multicast_server : public basic_multicast_base<ContentType>
	{
	public:
		typedef ContentType				content_type;
		typedef boost::asio::ip::udp	protocol_type;
		typedef protocol_type::socket	socket_type;
		typedef protocol_type::endpoint endpoint;
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
			auto message = multicast_protocol::build_announce_message(_token, _identifier, content);
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
			auto message = boost::make_shared<std::string>(multicast_protocol::build_announce_message(_token, _identifier, content));
			_socket.async_send_to(boost::asio::buffer(*message), _multicastEndpoint, _sendStrand.wrap(
				[innerHandler, message](const boost::system::error_code& ec, std::size_t sendLen) mutable
			{
				innerHandler(ec);
			}));

			return init.result.get();
		}

		void capture(endpoint& origin)
		{
			boost::system::error_code ec;
			auto res = discover(origin, ec);
			boost::asio::detail::throw_error(ec);
			return res;
		}

		boost::system::error_code capture(endpoint& origin, boost::system::error_code& ec)
		{
			result res;
			bool ok;
			do {
				auto len = _socket.receive_from(boost::asio::buffer(_recvBuffer), origin, 0, ec);
				XNET_RETURN_ERROR(ec);

				ok = _handle_capture(len);

			} while (!ok);

			return ec;
		}

		template<typename DiscoverHandler>
		BOOST_ASIO_INITFN_RESULT_TYPE(DiscoverHandler,
			void(boost::system::error_code))
			async_capture(endpoint& origin, BOOST_ASIO_MOVE_ARG(DiscoverHandler) handler)
		{
			boost::asio::detail::async_result_init<
				DiscoverHandler, void(boost::system::error_code)> init(
				BOOST_ASIO_MOVE_CAST(DiscoverHandler)(handler));

			_start_async_capture(origin, std::move(init.handler));

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

		template<typename Handler>
		void _start_async_capture(endpoint& origin, Handler handler)
		{
			_socket.async_receive_from(boost::asio::buffer(_recvBuffer), origin, 
				[handler, this, &origin](const boost::system::error_code& ec, std::size_t len) mutable
			{
				if ((ec != boost::asio::error::message_size && ec)
					|| _handle_capture(len))
				{
					handler(ec);
				}
				_start_async_capture(origin, std::move(handler));
			});
		}

		bool _handle_capture(std::size_t len)
		{
			return multicast_protocol::parse_scan_message(_recvBuffer.cbegin(), _recvBuffer.cbegin() + len, _token);
		}
	private:
		const std::string _identifier;
		boost::asio::io_service::strand _sendStrand;
		recvbuffer_type _recvBuffer;
	};

	typedef basic_multicast_server<multicast_string_content> multicast_server;
}




#endif