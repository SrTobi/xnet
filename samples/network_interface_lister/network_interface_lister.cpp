#include <iostream>
#include <xnet/network_interface_lister.hpp>


void print_network_interface(const boost::asio::ip::udp& protocol)
{
	boost::asio::io_service service;
	xnet::udp_interface_lister lister(service, protocol);

	for (auto& entry : lister)
	{
		std::cout << "  " << entry.host_name() << ": " << entry.endpoint().address() << std::endl;
	}
}

int main()
{
	std::cout << "Local ipv4 network interfaces:" << std::endl;
	print_network_interface(boost::asio::ip::udp::v4());


	std::cout << std::endl << std::endl;
	std::cout << "Local ipv6 network interfaces:" << std::endl;
	print_network_interface(boost::asio::ip::udp::v6());

#ifdef WIN32
	std::cin.get();
#endif

	return 0;
}