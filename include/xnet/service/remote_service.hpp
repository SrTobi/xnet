#pragma once
#ifndef _XNET_SERVICE_REMOTE_SERVICE_HPP
#define _XNET_SERVICE_REMOTE_SERVICE_HPP

#include <memory>
#include <assert.h>
#include "generic_service.hpp"
#include "service_peer.hpp"
#include "call_error.hpp"
#include "../package.hpp"
#include "../serialization/split.hpp"

namespace xnet {
	namespace service {

		template<typename Service>
		class remote_service
		{
			friend class serialization::access;
			static_assert(is_service<Service>::value, "Service must be a service!");
		public:
			remote_service()
			{
			}

			remote_service(std::nullptr_t)
			{
			}

			remote_service(std::shared_ptr<Service> impl)
				: _service(std::move(impl))
			{
				assert(!_service || !remote());
			}

			template<typename Service, typename Ret, typename... FArgs, typename... Args>
			void invoke(Ret(Service::*method)(FArgs...), Args&&... args)
			{
				static_assert(is_service<Service>::value, "Service must be a service!");
				static_assert(sizeof...(FArgs) == sizeof...(Args), "Wrong number of arguments provided!");
				static_assert(::xnet::detail::variadic_and<std::is_convertible<Args, FArgs>::value..., true>::value, "Provided arguments can not be converted to required types!");
				{
					assert(remote());
					assert(_service->_peer);
				}

				const auto& desc = get_descriptor<Service>();
				service_peer* peer = _service->_peer;

				std::tuple<typename detail::ref_or_val<FArgs, Args>::type...> arguments(args...);
				package arg_pack = peer->_factory->make_package(arguments, serialization::make_context(*peer));
				peer->_invoke(*_service, desc.resolve_method(method).id(), arg_pack);
			}

			template<typename Service, typename Ret, typename... FArgs, typename RetHandler, typename ExcpHandler, typename... Args>
			void call(
				Ret(Service::*method)(FArgs...),
				RetHandler&& handler,
				ExcpHandler&& excpHandler,
				Args&&... args)
			{
				static_assert(is_service<Service>::value, "Service must be a service!");
				static_assert(sizeof...(FArgs) == sizeof...(Args), "Wrong number of arguments provided!");
				static_assert(::xnet::detail::variadic_and<std::is_convertible<Args, FArgs>::value..., true>::value, "Provided arguments can not be converted to required types!");
				static_assert(std::is_convertible<ExcpHandler, std::function<void(call_error&&)>>::value, "RetHandler must be convertible to void(Ret)!");
				{
					std::function<void(call_error&&)> excpHandler2 = excpHandler;

					assert(remote());
					assert(_service->_peer);
				}

				const auto& desc = get_descriptor<Service>();
				service_peer* peer = _service->_peer;

				std::tuple<typename detail::ref_or_val<FArgs, Args>::type...> arguments(args...);
				package arg_pack = peer->_factory->make_package(arguments, serialization::make_context(*peer));
				peer->_call(*_service, desc.resolve_method(method).id(), peer->_make_return_id<Ret>(handler, excpHandler, std::is_void<Ret>()), arg_pack);
			}

			bool operator ==(const remote_service& other) const
			{
				return _service == other._service;
			}

			bool operator !=(const remote_service& other) const
			{
				return _service != other._service;
			}

			bool remote() const
			{
				return _service && !_service->_local;
			}

			operator bool() const
			{
				return (bool)_service;
			}

			bool operator !() const
			{
				return !_service;
			}

		private:
			template<typename S>
			void serialize(S& s)
			{
				serialization::split_this(this, s);
			}

			template<typename S>
			void load(S& s)
			{
				serviceid_type id;
				s >> id;
				if (id == 0)
				{
					_service = nullptr;
				}
				else{
					std::string checksum;
					s >> checksum;
					_service = s.context<service_peer>()._make_incoming_service<Service>(id, checksum);
				}
			}

			template<typename S>
			void save(S& s) const
			{
				if (!_service)
				{
					s << serviceid_type(0);
				}
				else{
					s << s.context<service_peer>()._make_outgoing_service(_service, typeid(Service));
					s << get_descriptor<Service>().checksum();
				}
			}
		public:
			std::shared_ptr<generic_service> _service;
		};

	}
}


#endif