#include "xnet/service/service_peer.hpp"
#include "service_protocol.hpp"

namespace xnet {
	namespace service {

		service_peer::service_peer(package_factory* factory)
			: _factory(factory)
		{
			assert(nullptr);
		}

		void service_peer::associate_service(std::shared_ptr<generic_service> service)
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
		}

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

		xnet::service::returnid_type service_peer::_create_return_id(std::function<void(const package&)>&& handler, std::function<void(call_error&&)> excpHandler)
		{
			throw std::exception("Not implemented!");
		}

		std::shared_ptr<generic_service> service_peer::_resolve_service(serviceid_type id, const std::string& checksum)
		{
			throw std::exception("Not implemented!");
		}

		xnet::package service_peer::_make_invokation_package(const std::string& serviceName, const std::string& checksum, funcid_type funcId, package arg_pack)
		{
			protocol::header header = protocol::static_invokation
			{
				serviceName,
				checksum,
				funcId
			};
			return _factory->make_package(header, arg_pack);
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
			return _factory->make_package(header, arg_pack);
		}

	}
}