#include "xnet/service/generic_service.hpp"

namespace xnet {
	namespace service {

		generic_service::generic_service()
		{

		}

		generic_service::~generic_service()
		{
		}

		generic_service::generic_service(remote_tag_t)
			: _local(false)
		{
		}
	}
}