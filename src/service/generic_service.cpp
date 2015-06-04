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
	}
}