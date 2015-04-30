#include "xnet/service/generic_service.hpp"

namespace xnet {
	namespace service {
		namespace detail {

			service_backend::service_backend(service_peer* peer, generic_service* service, serviceid_type id)
				: _peer(peer)
				, _service(service)
				, _id(id)
			{
			}

		}

		generic_service::~generic_service()
		{
		}
	}
}