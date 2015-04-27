#pragma once
#ifndef _XNET_SERVICE_SERVICE_PEER_HPP
#define _XNET_SERVICE_SERVICE_PEER_HPP

#include "service_descriptor.hpp"
#include "../package.hpp"

namespace xnet {
	namespace service {

		class service_peer
		{
		public:
			service_peer();

			template<typename Service, typename Ret, typename... Args>
			void make_invokation(const std::string& serviceNmae, Ret(Service::*method)(Args...), Args&&...)
			{

			}

			template<typename Service>
			void add_service(const std::string& serviceName, std::shared_ptr<Service> service)
			{

			}

			template<typename Service>
			std::shared_ptr<Service> get_service() const
			{

			}

			void remove_service(const std::string& serviceName);

			void process_package(const package&);
		private:

		};
	}
}


#endif