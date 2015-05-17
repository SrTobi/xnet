#include <stdexcept>
#include "remote_service_backend.hpp"


namespace xnet {
	namespace service {
		namespace internal {

			remote_service_backend::remote_service_backend(serviceid_type id)
				: generic_service(generic_service::remote_tag)
				, _id(id)
			{
			}

			const generic_service_descriptor& remote_service_backend::_descriptor() const
			{
				throw std::logic_error("can not get remote service's descriptor");
			}

			xnet::service::serviceid_type remote_service_backend::id() const
			{
				return _id;
			}

		}
	}
}