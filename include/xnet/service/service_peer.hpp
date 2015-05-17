#pragma once
#ifndef _XNET_SERVICE_SERVICE_PEER_HPP
#define _XNET_SERVICE_SERVICE_PEER_HPP

#include <unordered_map>
#include "../serialization/serialization.hpp"
#include "../serialization/types/tuple.hpp"
#include "../detail/variadic_utils.hpp"
#include "service_descriptor.hpp"
#include "call_error.hpp"
#include "../package.hpp"
#include "../package_factory.hpp"

namespace xnet {
	namespace service {
		template<typename Service>
		class remote_service;

		class service_peer
		{
			template<typename Service>
			friend class remote_service;
			template<typename Service, typename Method>
			class detail::specific_service_method_descriptor;

			struct return_slot
			{
				std::function<void(const package&)> handler;
				std::function<void(call_error&&)> excpHandler;
			};
		public:
			service_peer(package_factory* factory);

			template<typename Service, typename Ret, typename... FArgs, typename... Args>
			package make_invokation(const std::string& serviceName, Ret(Service::*method)(FArgs...), Args&&... args)
			{
				static_assert(is_service<Service>::value, "Service must be a service!");
				static_assert(sizeof...(FArgs) == sizeof...(Args), "Wrong number of arguments provided!");
				static_assert(::xnet::detail::variadic_and<std::is_convertible<Args, FArgs>::value..., true>::value, "Provided arguments can not be converted to required types!");

				const auto& desc = get_descriptor<Service>();

				std::tuple<FArgs...> arguments(args...);
				package arg_pack = _factory->make_package(arguments, serialization::make_context(*this));
				return _make_invokation_package(serviceName, desc.checksum(), desc.resolve_method(method).id(), arg_pack);
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
				static_assert(::xnet::detail::variadic_and<std::is_convertible<Args, FArgs>::value..., true>::value, "Provided arguments can not be converted to required types!");
				static_assert(std::is_convertible<RetHandler, std::function<void(Ret&&)>>::value, "RetHandler must be convertible to void(Ret)!");
				static_assert(std::is_convertible<ExcpHandler, std::function<void(call_error&&)>>::value, "RetHandler must be convertible to void(Ret)!");
				{
					std::function<void(Ret&&)> handler2 = handler;
					std::function<void(call_error&&)> excpHandler2 = excpHandler;
				}

				const auto& desc = get_descriptor<Service>();

				std::tuple<FArgs...> arguments(args...);
				package arg_pack = _factory->make_package(arguments, serialization::make_context(*this));
				return _make_call_package(serviceName, desc.checksum(), desc.resolve_method(method).id(), _make_return_id<Ret>(handler, excpHandler), arg_pack);
			}

			template<typename Service, typename Ret, typename... FArgs, typename RetHandler, typename ExcpHandler, typename... Args>
			package make_call(
				const remote_service<Service>& service,
				Ret(Service::*method)(FArgs...),
				RetHandler&& handler,
				ExcpHandler&& excpHandler,
				Args&&... args)
			{
				static_assert(is_service<Service>::value, "Service must be a service!");
				static_assert(sizeof...(FArgs) == sizeof...(Args), "Wrong number of arguments provided!");
				static_assert(::xnet::detail::variadic_and<std::is_convertible<Args, FArgs>::value..., true>::value, "Provided arguments can not be converted to required types!");
				static_assert(std::is_convertible<RetHandler, std::function<void(Ret&&)>>::value, "RetHandler must be convertible to void(Ret)!");
				static_assert(std::is_convertible<ExcpHandler, std::function<void(call_error&&)>>::value, "RetHandler must be convertible to void(Ret)!");
				{
					std::function<void(Ret&&)> handler2 = handler;
					std::function<void(call_error&&)> excpHandler2 = excpHandler;

					assert(service.remote());
				}

				const auto& desc = get_descriptor<Service>();

				std::tuple<FArgs...> arguments(args...);
				package arg_pack = _factory->make_package(arguments, serialization::make_context(*this));
				return _make_call_package(*service._service, desc.resolve_method(method).id(), _make_return_id<Ret>(handler, excpHandler), arg_pack);
			}

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
				static_assert(is_service<Service>::value || std::is_same<Service, generic_service>::value, "Service must be a service/generic service!");
				auto it = _namedServices.find(name);
				if (it == _namedServices.end())
					return nullptr;

				return std::dynamic_pointer_cast<Service>(it->second);
			}

			bool remove_service(const std::string& name);

			package process_package(const package&);

			template<typename Ret>
			package _make_return_content_package(const Ret& return_value, returnid_type retId)
			{
				auto content_package = _factory->make_package(XNET_TAGVAL(return_value), serialization::make_context(*this));
				return _make_return_invokation_package(content_package, retId);
			}
			package _make_exception_package(const std::exception& e, returnid_type retId);
		private:

			template<typename Ret, typename RetHandler, typename ExcpHandler>
			returnid_type _make_return_id(RetHandler&& handler, ExcpHandler&& excpHandler)
			{
				auto rh = [handler, this](const package& p)
				{
					Ret return_value;
					p.deserialize(XNET_TAGVAL(return_value), serialization::make_context(*this), package::try_next_package);
					handler(std::move(return_value));
				};

				return _create_return_id(std::move(rh), excpHandler);
			}

			serviceid_type _newServiceId();
			returnid_type _newReturnId();

			returnid_type _create_return_id(std::function<void(const package&)>&& handler, std::function<void(call_error&&)> excpHandler);

			template<typename Service>
			std::shared_ptr<generic_service> _make_incoming_service(serviceid_type id, const std::string& checksum)
			{
				return _make_incoming_service(id, checksum, get_descriptor<Service>().checksum());
			}
			std::shared_ptr<generic_service> _make_incoming_service(serviceid_type id, const std::string& checksum, const std::string& desc_checksum);
			template<typename Service>
			serviceid_type _make_outgoing_service(const std::shared_ptr<const Service>& service)
			{
				return _make_outgoing_service(service, typeid(Service));
			}
			serviceid_type _make_outgoing_service(const std::shared_ptr<generic_service>& service, const std::type_info& info);

			package _make_return_invokation_package(const package& content, returnid_type retId);
			package _make_invokation_package(const std::string& serviceName, const std::string& checksum, funcid_type funcId, package arg_pack);
			package _make_call_package(const std::string& serviceName, const std::string& checksum, funcid_type funcId, returnid_type returnId, package arg_pack);
			package _make_call_package(const generic_service& service, funcid_type funcId, returnid_type returnId, package arg_pack);
		private:
			std::unordered_map<std::shared_ptr<generic_service>, serviceid_type> _outgoingServices;
			std::unordered_map<serviceid_type, std::shared_ptr<generic_service>> _outgoingServicesById;
			std::unordered_map<serviceid_type, std::shared_ptr<generic_service>> _incomingServices;
			std::unordered_map<returnid_type, return_slot> _returnSlots;
			std::unordered_map<std::string, std::shared_ptr<generic_service>> _namedServices;
			package_factory* _factory;
			serviceid_type _nextServiceId = 1;
			returnid_type _nextReturnId = 1;
		};
	}
}


#endif