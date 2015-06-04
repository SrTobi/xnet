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
				remote_service_backend(serviceid_type id, service_peer* peer);
				virtual ~remote_service_backend();

				virtual const generic_service_descriptor& _descriptor() const override final;
				serviceid_type id() const;
				void detach();
			private:
				serviceid_type _id;
				service_peer* _peer;
			};

		}
	}
}


#endif