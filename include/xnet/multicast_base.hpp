#pragma once
#ifndef _XNET_MULTICAST_BASE_HPP
#define _XNET_MULTICAST_BASE_HPP

#include <boost/regex.hpp>
#include <boost/asio.hpp>
#include <boost/assert.hpp>
#include "detail/return_on_error.hpp"
#include "detail/finnaly.hpp"
#include "detail/multicast_protocol.hpp"
#include "network_interface_lister.hpp"

namespace xnet {

	template<typename ContentType>
	class basic_multicast_base
	{
	public:
		typedef ContentType				content_type;
		typedef boost::asio::ip::udp	protocol_type;
		typedef protocol_type::socket	socket_type;
		typedef protocol_type::endpoint endpoint;

	private:
		typedef detail::multicast_protocol<content_type> multicast_protocol;

	public:
		basic_multicast_base(boost::asio::io_service& service, const std::string& recognitionToken)
			: _service(service)
			, _socket(service)
			, _token(recognitionToken)
		{
		}

		void open(const protocol_type& protocol)
		{
			endpoint ep(protocol, 0);
			open(ep.address());
		}

		void open(const boost::asio::ip::address& listenAddress)
		{
			boost::system::error_code ec;
			open(listenAddress, ec);
			boost::asio::detail::throw_error(ec);
		}

		void open(const endpoint& multicastEndpoint)
		{
			boost::system::error_code ec;
			open(multicastEndpoint, ec);
			boost::asio::detail::throw_error(ec);
		}

		void open(const endpoint& multicastEndpoint, const boost::asio::ip::address& listenAddress)
		{
			boost::system::error_code ec;
			open(multicastEndpoint, listenAddress, ec);
			boost::asio::detail::throw_error(ec);
		}

		boost::system::error_code open(const protocol_type& listenAddress, boost::system::error_code& ec)
		{
			const endpoint multicastEndpoint(
				multicast_protocol::generate_address(listenAddress.is_v4(), _token),
				multicast_protocol::generate_port(_token));
			open(multicastEndpoint, listenAddress, ec);
			return ec;
		}


		boost::system::error_code open(const boost::asio::ip::address& listenAddress, boost::system::error_code& ec)
		{
			const endpoint multicastEndpoint(
				multicast_protocol::generate_address(listenAddress.is_v4(), _token),
				multicast_protocol::generate_port(_token));
			open(multicastEndpoint, listenAddress, ec);
			return ec;
		}

		boost::system::error_code open(const endpoint& multicastEndpoint, boost::system::error_code& ec)
		{
			boost::asio::ip::address addr;
			if (multicastEndpoint.address().is_v4())
				addr = boost::asio::ip::address_v4::any();
			else
				addr = boost::asio::ip::address_v6::any();

			open(multicastEndpoint, addr, ec);

			return ec;
		}

		boost::system::error_code open(const endpoint& multicastEndpoint, const boost::asio::ip::address& listenAddress, boost::system::error_code& ec) 
		{
			BOOST_ASSERT(multicastEndpoint.address().is_v4() == listenAddress.is_v4());
			detail::finally fin = [&ec, this]()
			{
				if (ec)
				{
					_socket.close();
				}
			};
			_socket.open(multicastEndpoint.protocol(), ec);
			XNET_RETURN_ERROR(ec);

			_socket.set_option(boost::asio::ip::multicast::enable_loopback(true));
			_socket.set_option(boost::asio::ip::udp::socket::reuse_address(true));
			_socket.bind(endpoint(listenAddress, multicastEndpoint.port()), ec);
			XNET_RETURN_ERROR(ec);

			_multicastEndpoint = multicastEndpoint;

			if (listenAddress.is_v4())
			{
				udp_interface_lister il(_service, boost::asio::ip::udp::v4());

				for (auto& iaddr : il)
				{
					_socket.set_option(boost::asio::ip::multicast::join_group(multicastEndpoint.address().to_v4(), iaddr.endpoint().address().to_v4()), ec);
					XNET_RETURN_ERROR(ec);
				}
			}
			else{
				_socket.set_option(boost::asio::ip::multicast::join_group(multicastEndpoint.address()), ec);
				XNET_RETURN_ERROR(ec);
			}

			return ec;
		}

		bool is_open() const
		{
			return _socket.is_open();
		}

		void close()
		{
			_socket.close();
		}

		boost::system::error_code close(boost::system::error_code& ec)
		{
			return _socket.close(ec);
		}

		boost::asio::io_service& io_service() const { return _service; }
		socket_type& underlying_socket() { return _socket; }
		const socket_type& underlying_socket() const { return _socket; }
		const endpoint& multicast_endpoint() const { return _multicastEndpoint; }

		const std::string& recognition_token() const { return _token; }
	protected:
		const std::string _token;
		boost::asio::io_service& _service;

		socket_type _socket;
		endpoint _multicastEndpoint;
	};



}



#endif