#pragma once
#ifndef _XNET_SERVICE_SERVICE_HPP
#define _XNET_SERVICE_SERVICE_HPP

#include <memory>
namespace xnet {
	namespace service
	{
		typedef uint16_t funcid_type;
		typedef uint32_t returnid_type;
		typedef uint16_t serviceid_type;

		class generic_service
			: public std::enable_shared_from_this<generic_service>
		{
		public:
			virtual ~generic_service();

		private:
			serviceid_type _serviceId = 0;
		};

		template<typename Service>
		struct is_service
			: public std::is_base_of<generic_service, Service>
		{
		};
	}
}


#endif