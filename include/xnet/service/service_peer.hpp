#pragma once
#ifndef _XNET_SERVICE_SERVICE_PEER_HPP
#define _XNET_SERVICE_SERVICE_PEER_HPP

#include <unordered_map>
#include "../detail/variadic_utils.hpp"
#include "protocol/service_protocol.hpp"
#include "service_descriptor.hpp"
#include "call_error.hpp"
#include "../package.hpp"
#include "../package_factory.hpp"

namespace xnet {
	namespace service {

		class service_peer
		{
			template<typename Service>
			friend class remote_service;
		public:
			service_peer();

			template<typename Service, typename FArgs, typename... Args>
			package make_invokation(const std::string& serviceName, void(Service::*method)(Args...), Args&&... args)
			{
				static_assert(is_service<Service>::value, "Service must be a service!");
				static_assert(sizeof...(FArgs) == sizeof...(Args), "Wrong number of arguments provided!");
				static_assert(::xnet::detail::variadic_and<std::is_convertible<Args, FArgs>::value...>::value, "Provided arguments can not be converted to required types!");

				const auto& desc = get_descriptor<Service>();

				protocol::invokation_type invk_type = protocol::invokation_type::static_invokation;
				protocol::static_invokation<FArgs...> invk
				{
					serviceName,
					desc.checksum(),
					desc.resolve_method(method),
					std::forward_as_tuple(args...)
				};

				package content_pack = _factory->make_package(invk);
				return _factory->make_package(invk_type, content_pack);
			}

			template<typename Service, typename Ret, typename... FArgs, typename RetHandler, typename ExcpHandler, typename... Args>
			package make_call(
				const std::string& serviceName,
				Ret(Service::*method)(FArgs...),
				RetHandler&& handler,
				ExcpHandler&& excpHandler,
				Args&&... args)
			{
				static_assert(is_service<Service>::value, "Service must be a service!");
				static_assert(sizeof...(FArgs) == sizeof...(Args), "Wrong number of arguments provided!");
				static_assert(::xnet::detail::variadic_and<std::is_convertible<Args, FArgs>::value...>::value, "Provided arguments can not be converted to required types!");
				static_assert(std::is_convertible<RetHandler, std::function<void(Ret&&)>>::value, "RetHandler must be convertible to void(Ret)!");
				static_assert(std::is_convertible<ExcpHandler, std::function<void(call_error&&)>>::value, "RetHandler must be convertible to void(Ret)!");
				std::function<void(Ret&&)> func = handler;
				std::function<void(call_error&&)> func2 = excpHandler;

				const auto& desc = get_descriptor<Service>();

				protocol::invokation_type invk_type = protocol::invokation_type::static_call;
				protocol::static_call<FArgs...> call
				{
					serviceName,
					desc.checksum(),
					desc.resolve_method(method),
					_make_return_id<Ret>(handler, excpHandler),
					std::forward_as_tuple(args...)
				};

				package content_pack = _factory->make_package(call, serialization::make_context(*this));
				return _factory->make_package(invk_type, content_pack);
			}

			void associate_service(std::shared_ptr<generic_service> service);
			/*{
				static_assert(is_service<Service>::value, "Service must be a service!");
				assert(service);
				assert(service->_local);
				if (service->_servicePeer)
				{
					if (service->_servicePeer == this)
						return;

					throw std::logic_error("Service is already associated with another peer!");
				}

				_associate_service()
			}*/

			template<typename Service>
			void add_service(const std::string& name, std::shared_ptr<Service> service)
			{
				static_assert(is_service<Service>::value, "Service must be a service!");

				auto res = _namedServices.emplace(name, std::move(service));
				if (!res.second)
				{
					throw std::logic_error("A service with the name '" + name + "' is already registered!");
				}
			}

			template<typename Service>
			std::shared_ptr<Service> get_service(const std::string& name) const
			{
				static_assert(is_service<Service>::value, "Service must be a service!");
				auto it = _namedServices.find(name);
				if (it == _namedServices.end())
					return nullptr;

				return std::dynamic_pointer_cast<Service>(it->second);
			}

			bool remove_service(const std::string& name);

			package process_package(const package&);
		private:
			template<typename Ret, typename RetHandler, typename ExcpHandler>
			returnid_type _make_return_id(RetHandler&& handler, ExcpHandler&& excpHandler)
			{
				auto rh = [handler, this](const package& p)
				{
					protocol::return_invokation<Ret> ri;
					p.deserialize(ri, serialization::make_context(*this));
					handler(std::move(ri.return_value));
				};

				return _create_return_id(std::move(rh), excpHandler);
			}

			returnid_type _create_return_id(std::function<void(const package&)>&& handler, std::function<void(call_error&&)> excpHandler);
			std::shared_ptr<generic_service> _resolve_service(serviceid_type id, const std::string& checksum);
		private:
			std::unordered_map<std::string, std::shared_ptr<generic_service>> _namedServices;
			package_factory* _factory;
		};
	}
}


#endif