#include "xnet/service/service_peer.hpp"
#include "remote_service_backend.hpp"
#include "service_protocol.hpp"

namespace xnet {
	namespace service {

		service_peer::service_peer(package_factory* factory, const emit_function& emitFunc)
			: _factory(factory)
			, _emit(emitFunc)
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

		void service_peer::process_package(const package& pack)
		{
			struct header_visitor : public boost::static_visitor<void>
			{
				header_visitor(service_peer* peer, const package& args)
					: peer(peer)
					, args(args)
				{
				}

				void operator()(protocol::static_invokation& call)
				{
					std::shared_ptr<generic_service> service;
					auto& desc = _static_descriptor(call.service_name, call.service_checksum, service);
					desc.invoke(service, *peer, call.func_id, args);
				}

				void operator()(protocol::static_call& call)
				{
					std::shared_ptr<generic_service> service;
					auto& desc = _static_descriptor(call.service_name, call.service_checksum, service);
					peer->_emit(desc.call(service, *peer, call.func_id, call.return_id, args));
				}

				void operator()(protocol::dynamic_invokation& call)
				{
					std::shared_ptr<generic_service> service;
					auto& desc = _dynamic_descriptor(call.service_id, service);
					desc.invoke(service, *peer, call.func_id, args);
				}

				void operator()(protocol::dynamic_call& call)
				{
					std::shared_ptr<generic_service> service;
					auto& desc = _dynamic_descriptor(call.service_id, service);
					peer->_emit(desc.call(service, *peer, call.func_id, call.return_id, args));
				}

				void operator()(protocol::return_invokation& call)
				{
					auto& slots = peer->_returnSlots;
					auto it = slots.find(call.return_id);
					if (it == slots.end())
						throw std::runtime_error("unknown return id");

					it->second.handler(args);
					slots.erase(it);
				}
				
				void operator()(protocol::return_exception& call)
				{
					auto& slots = peer->_returnSlots;
					auto it = slots.find(call.return_id);
					if (it == slots.end())
						throw std::runtime_error("unknown return id");

					it->second.excpHandler(call.message);
				}

				service_peer* peer;
				const package& args;

			private:
				const generic_service_descriptor& _static_descriptor(const std::string& serviceName, const std::string& serviceChecksum, std::shared_ptr<generic_service>& service)
				{
					service = peer->get_service<generic_service>(serviceName);
					if (!service)
					{
						// TODO: throw exception?!
						throw std::runtime_error("A static service with name '" + serviceName + "' does not exist!");
					}
					
					const auto& desc = service->_descriptor();
					
					if (serviceChecksum != desc.checksum())
					{
						// TODO: throw exception?!
						throw std::runtime_error("Static service has different checksum!");
					}

					return desc;
				}

				const generic_service_descriptor& _dynamic_descriptor(serviceid_type serviceId, std::shared_ptr<generic_service>& service)
				{
					auto it = peer->_outgoingServicesById.find(serviceId);
					if (it == peer->_outgoingServicesById.end())
					{
						// TODO: throw exception?!
						throw std::runtime_error("unknown service id");
					}
					service = it->second;

					return service->_descriptor();
				}
			};

			protocol::header header;
			package args;
			args = pack.deserialize(XNET_TAGVAL(header), serialization::make_context(*this), package::try_next_package);

			header_visitor v
			{
				this,
				args
			};
			header.apply_visitor(v);
		}

		std::size_t service_peer::open_return_slots() const
		{
			return _returnSlots.size();
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

		std::shared_ptr<generic_service> service_peer::_make_incoming_service(serviceid_type id, const std::string& checksum, const std::string& desc_checksum)
		{
			if (checksum != desc_checksum)
			{
				throw std::runtime_error("wrong checksum");
			}
			auto it = _incomingServices.find(id);

			if (it == _incomingServices.end())
			{
				auto res = std::make_shared<internal::remote_service_backend>(id);
				_incomingServices.emplace(id, res);
				return res;
			}

			return  it->second;
		}

		xnet::service::serviceid_type service_peer::_make_outgoing_service(const std::shared_ptr<generic_service>& service, const std::type_info& info)
		{
			auto it = _outgoingServices.find(service);
			if (it != _outgoingServices.end())
				return it->second;

			auto id = _newServiceId();
			bool inserted;
			std::tie(std::ignore, inserted) = _outgoingServices.emplace(service, id);
			assert(inserted);

			std::tie(std::ignore, inserted) = _outgoingServicesById.emplace(id, std::move(service));
			if (!inserted)
			{
				throw std::runtime_error("Can not associate service! There are too many services associated!");
			}

			return id;
		}

		xnet::package service_peer::_make_return_invokation_package(const package& content, returnid_type retId)
		{
			protocol::header header = protocol::return_invokation
			{
				retId
			};
			return _factory->make_package(XNET_TAGVAL(header), content);
		}

		void service_peer::_invoke(const std::string& serviceName, const std::string& checksum, funcid_type funcId, package arg_pack)
		{
			protocol::header header = protocol::static_invokation
			{
				serviceName,
				checksum,
				funcId
			};
			_emit(_factory->make_package(XNET_TAGVAL(header), arg_pack));
		}

		void service_peer::_invoke(const generic_service& service, funcid_type funcId, package arg_pack)
		{
			assert(!service._local);
			const auto& backend = static_cast<const internal::remote_service_backend&>(service);
			
			protocol::header header = protocol::dynamic_invokation
			{
				backend.id(),
				funcId
			};
			_emit(_factory->make_package(XNET_TAGVAL(header), arg_pack));
		}

		void service_peer::_call(const std::string& serviceName, const std::string& checksum, funcid_type funcId, returnid_type returnId, package arg_pack)
		{
			protocol::header header = protocol::static_call
			{
				serviceName,
				checksum,
				funcId,
				returnId
			};
			_emit(_factory->make_package(XNET_TAGVAL(header), arg_pack));
		}

		void service_peer::_call(const generic_service& service, funcid_type funcId, returnid_type returnId, package arg_pack)
		{
			assert(!service._local);
			const auto& backend = static_cast<const internal::remote_service_backend&>(service);
			
			protocol::header header = protocol::dynamic_call
			{
				backend.id(),
				funcId,
				returnId
			};
			_emit(_factory->make_package(XNET_TAGVAL(header), arg_pack));
		}

		xnet::package service_peer::_make_exception_package(const std::exception& e, returnid_type retId)
		{
			protocol::header header  = protocol::return_exception
			{
				retId,
				e.what()
			};
			return _factory->make_package(XNET_TAGVAL(header));
		}
	}
}