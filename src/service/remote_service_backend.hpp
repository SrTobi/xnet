#pragma once
#ifndef _XNET_SERVICE_REMOTE_SERVICE_BACKEND_HPP
#define _XNET_SERVICE_REMOTE_SERVICE_BACKEND_HPP

#include "xnet/service/generic_service.hpp"

namespace xnet {
	namespace service {
		namespace internal {

			class remote_service_backend
				: public generic_service
			{
			public:
				remote_service_backend(serviceid_type id);

				virtual const generic_service_descriptor& _descriptor() const override final;
				serviceid_type id() const;
			private:
				serviceid_type _id;
			};

		}
	}
}


#endif