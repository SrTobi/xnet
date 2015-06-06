#pragma once
#ifndef _XNET_PACKAGE_SOCKET_ADAPTER_HPP
#define _XNET_PACKAGE_SOCKET_ADAPTER_HPP

#include <boost/asio/buffer.hpp>
#include <boost/asio/ip/tcp.hpp>
#include "package_factory.hpp"


namespace xnet {

	template<typename Socket>
	class package_socket_adapter
	{
	public:
		typedef typename Socket::lowest_layer_type lowest_layer_type;
		typedef Socket next_layer_type;
	public:
		template<typename... Args>
		package_socket_adapter(Args&&... args)
			: _socket(std::forward<Args>(args)...)
		{
		}

		std::size_t send(const package& pack)
		{
			boost::system::error_code ec;
			auto s = send(pack, ec);
			boost::asio::detail::throw_error(ec);
			return s;
		}

		std::size_t send(const package& pack, boost::system::error_code& ec)
		{
			return _socket.send(pack.underlying_data(), ec);
		}

		template<typename SendHandler>
		BOOST_ASIO_INITFN_RESULT_TYPE(SendHandler,
			void(boost::system::error_code, std::size_t))
			async_send(const package& pack, BOOST_ASIO_MOVE_ARG(SendHandler) handler)
		{
			boost::asio::detail::async_result_init<
				SendHandler, void(boost::system::error_code, std::size_t)> init(
				BOOST_ASIO_MOVE_CAST(SendHandler)(handler));

			auto innerHandler = std::move(init.handler);
			_socket.async_send(pack.underlying_data(),
				[innerHandler, pack](const boost::system::error_code& ec, std::size_t sendLen) mutable
				{
					innerHandler(ec, sendLen);
				});

			return init.result.get();
		}


		const next_layer_type& next_layer() const
		{
			return _socket;
		}

		next_layer_type& next_layer()
		{
			return _socket;
		}

		const lowest_layer_type& lowest_layer() const
		{
			return _socket.lowest_layer();
		}

		lowest_layer_type& lowest_layer()
		{
			return _socket.lowest_layer();
		}

	private:
		Socket _socket;

	};

}


#endif