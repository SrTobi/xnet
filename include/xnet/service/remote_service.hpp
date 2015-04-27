#pragma once
#ifndef _XNET_SERVICE_REMOTE_SERVICE_HPP
#define _XNET_SERVICE_REMOTE_SERVICE_HPP

#include <memory>


namespace xnet {
	namespace service {
		template<typename Service>
		class remote_service
		{
		public:
			remote_service(std::shared_ptr<Service> impl)
			{

			}

			template<typename Ret, typename... Args>
			void make_invokation(Ret(Service::*method)(Args...), Args&&... args)
			{

			}
		};

	}
}


#endif