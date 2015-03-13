#pragma once
#ifndef _XNET_BASIC_MULTICAST_CLINET_HPP
#define _XNET_BASIC_MULTICAST_CLIENT_HPP


#include "multicast_base.hpp"
#include "lan_discovery_content.hpp"

namespace xnet {

	template<typename ContentType>
	class basic_multicast_client : public basic_multicast_base<ContentType>
	{
	public:
		typedef ContentType				content_type;
		typedef boost::asio::ip::udp	protocol_type;
		typedef protocol_type::socket	socket_type;
		typedef protocol_type::endpoint endpoint_type;

		class result
		{
			template<typename Tmpl>
			friend class basic_multicast_client;
		public:
			result()
			{
			}

			result(const endpoint_type& from)
				: _recvfrom(from)
			{
			}

			const content_type& content() const { return _content; }
			const endpoint_type& origin() const { return _recvfrom; }
			const std::string& identifier() const { return _identifier; }

		private:
			content_type _content;
			endpoint_type _recvfrom;
			std::string _identifier;
		};

	private:
		typedef std::array<char, 2048> recvbuffer_type;

	public:
		basic_multicast_client(boost::asio::io_service& service, const std::string& token)
			: basic_multicast_base(service, token)
			, _sendStrand(service)
		{
		}

		~basic_multicast_client()
		{
		}

		void scan()
		{
			boost::system::error_code ec;
			scan(port, ec);
			boost::asio::detail::throw_error(ec);
		}

		boost::system::error_code scan(boost::system::error_code& ec)
		{
			auto message = _build_message(Command::Scan);
			_socket.send(boost::asio::buffer(message), ec);
			return ec;
		}
		
		result discover()
		{
			boost::system::error_code ec;
			auto res = discover(ec);
			boost::asio::detail::throw_error(ec);
			return res;
		}

		result discover(boost::system::error_code& ec)
		{
			result res;
			bool ok;
			do {
				auto len = _socket.receive_from(boost::asio::buffer(_recvBuffer), _recvEndpoint, 0, ec);
				XNET_RETURN_ON_ERROR(ec, res);

				ok = _handle_receive(len, res);

			} while (!ok);

			return res;
		}

		template<typename DiscoverHandler>
		BOOST_ASIO_INITFN_RESULT_TYPE(DiscoverHandler,
			void(boost::system::error_code, result))
			async_discover(BOOST_ASIO_MOVE_ARG(DiscoverHandler) handler)
		{
			boost::asio::detail::async_result_init<
				DiscoverHandler, void(boost::system::error_code, result)> init(
				BOOST_ASIO_MOVE_CAST(DiscoverHandler)(handler));

			_start_async_discover(std::move(init.handler));

			return init.result.get();
		}

	private:
		template<typename Handler>
		void _start_async_discover(Handler handler)
		{
			_socket.async_receive_from(boost::asio::buffer(_recvBuffer), _recvEndpoint, 
				[handler, this](const boost::system::error_code& ec, std::size_t len) mutable
			{
				if(ec != boost::asio::error::message_size)
				{
					result res;
					if (ec || _handle_receive(len, res))
					{
						handler(ec, res);
					}
				}
				_start_async_discover(std::move(handler));
			});
		}

		bool _handle_receive(std::size_t len, result& res)
		{
			multicast_protocol::Command c;
			res = result(_recvEndpoint);
			return (multicast_protocol::parse_message(_recvBuffer.cbegin(), _recvBuffer.cbegin() + len, _token, res._identifier, c, res._content)
				&& c == multicast_protocol::Command::Announce);
		}

	private:
		boost::asio::io_service::strand _sendStrand;

		recvbuffer_type _recvBuffer;
		endpoint_type _recvEndpoint;
	};

	typedef basic_multicast_client<lan_discovery_content> multicast_client;
}




#endif