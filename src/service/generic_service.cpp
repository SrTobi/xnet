#include "xnet/service/service_peer.hpp"
#include "xnet/service/generic_service.hpp"

namespace xnet {
	namespace service {

		generic_service::generic_service()
		{

		}

		generic_service::~generic_service()
		{
		}

		generic_service::generic_service(service_peer* peer)
			: _local(false)
			, _peer(peer)
		{
			assert(_peer);
		}

		service_peer generic_service::call_origin() const
		{
			assert(_local);
			assert(_peer);
			return *_peer;
		}

	}
}