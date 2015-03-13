#pragma once
#ifndef _XNET_NETWORK_INTERFACE_LISTER_HPP
#define _XNET_NETWORK_INTERFACE_LISTER_HPP

#include <boost/asio.hpp>

namespace xnet {
	
	template<typename InternetProtocol>
	class basic_network_interface_lister
	{
	public:
		typedef InternetProtocol protocol_type;
		typedef typename protocol_type::resolver	resolver;
		typedef typename resolver::query			query;
		typedef typename resolver::iterator			iterator;
	public:
		basic_network_interface_lister(boost::asio::io_service& service)
			: _resolver(service)
			, _query("")
		{
		}

		basic_network_interface_lister(boost::asio::io_service& service, const protocol_type& protocol)
			: _resolver(service)
			, _query(protocol, "")
		{
		}

		iterator begin()
		{
			return _resolver.resolve(_query);
		}

		iterator end()
		{
			return iterator();
		}

	private:
		resolver _resolver;
		query _query;
	};

	typedef basic_network_interface_lister<boost::asio::ip::udp> udp_interface_lister;
	typedef basic_network_interface_lister<boost::asio::ip::tcp> tcp_interface_lister;



}





#endif