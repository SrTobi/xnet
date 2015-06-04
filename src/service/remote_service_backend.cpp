#include <stdexcept>
#include <xnet/service/service_peer.hpp>
#include "remote_service_backend.hpp"

namespace xnet {
	namespace service {
		namespace internal {

			remote_service_backend::remote_service_backend(serviceid_type id, service_peer* peer)
				: generic_service(peer)
				, _id(id)
			{
				assert(_id);
				assert(_peer);
			}

			remote_service_backend::~remote_service_backend()
			{
				if (_peer)
				{
					_peer->_notify_remote_service_release(_id);
				}
				else
				{
					assert(!_id);
				}
			}

			const generic_service_descriptor& remote_service_backend::_descriptor() const
			{
				throw std::logic_error("can not get remote service's descriptor");
			}

			xnet::service::serviceid_type remote_service_backend::id() const
			{
				return _id;
			}

			void remote_service_backend::detach()
			{
				_id = 0;
				_peer = nullptr;
			}
		}
	}
}