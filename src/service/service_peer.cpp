#include "xnet/service/service_peer.hpp"
#include "service_protocol.hpp"

namespace xnet {
	namespace service {

		service_peer::service_peer(package_factory* factory)
			: _factory(factory)
		{
			assert(factory);
		}

		/*void service_peer::associate_service(std::shared_ptr<generic_service> service)
		{
			assert(service);
			assert(service->_local);
			if (service->_backend)
			{
				if (service->_backend->_peer == this)
					return;

				throw std::logic_error("Service is already associated with another peer!");
			}

			auto id = _newServiceId();
			auto backend = std::unique_ptr<detail::service_backend>(new detail::service_backend{
				this,
				service.get(),
				id
			});

			bool inserted;
			std::tie(std::ignore, inserted) = _associatedServices.emplace(id, std::move(service));
			if (!inserted)
			{
				throw std::runtime_error("Can not associate service! There are too many services associated!");
			}

			service->_backend = std::move(backend);
		}*/

		bool service_peer::remove_service(const std::string& name)
		{
			return _namedServices.erase(name) > 0;
		}

		xnet::package service_peer::process_package(const package&)
		{
			throw std::exception("Not implemented!");
		}

		xnet::service::serviceid_type service_peer::_newServiceId()
		{
			auto newId = _nextServiceId++;
			return newId == 0 ? _newServiceId() : newId;
		}

		xnet::service::returnid_type service_peer::_newReturnId()
		{
			if(_nextReturnId == 0)
				throw std::runtime_error("Exceeded maximum number of returns!");
			return _nextReturnId++;
		}

		xnet::service::returnid_type service_peer::_create_return_id(std::function<void(const package&)>&& handler, std::function<void(call_error&&)> excpHandler)
		{
			auto id = _newReturnId();
			_returnSlots.emplace(id, return_slot{ handler, std::move(excpHandler) });
			return id;
		}

		std::shared_ptr<generic_service> service_peer::_resolve_service_id(serviceid_type id, const std::string& checksum)
		{
			return _idToServiceMapping.at(id);
		}

		xnet::service::serviceid_type service_peer::_get_service_id(const std::shared_ptr<generic_service>& service, const std::type_info& info)
		{
			auto it = _serviceToIdMapping.find(service);
			if (it != _serviceToIdMapping.end())
				return it->second;

			auto id = _newServiceId();
			bool inserted;
			std::tie(std::ignore, inserted) = _serviceToIdMapping.emplace(service, id);
			assert(inserted);

			std::tie(std::ignore, inserted) = _idToServiceMapping.emplace(id, std::move(service));
			if (!inserted)
			{
				throw std::runtime_error("Can not associate service! There are too many services associated!");
			}

			return id;
		}

		xnet::package service_peer::_make_invokation_package(const std::string& serviceName, const std::string& checksum, funcid_type funcId, package arg_pack)
		{
			protocol::header header = protocol::static_invokation
			{
				serviceName,
				checksum,
				funcId
			};
			return _factory->make_package(XNET_TAGVAL(header), arg_pack);
		}

		xnet::package service_peer::_make_call_package(const std::string& serviceName, const std::string& checksum, funcid_type funcId, returnid_type returnId, package arg_pack)
		{
			protocol::header header = protocol::static_call
			{
				serviceName,
				checksum,
				funcId,
				returnId
			};
			return _factory->make_package(XNET_TAGVAL(header), arg_pack);
		}

	}
}