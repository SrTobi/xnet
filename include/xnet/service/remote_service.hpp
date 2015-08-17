#pragma once
#ifndef _XNET_SERVICE_REMOTE_SERVICE_HPP
#define _XNET_SERVICE_REMOTE_SERVICE_HPP

#include <memory>
#include <assert.h>
#include "generic_service.hpp"
#include "call_error.hpp"
#include "../package.hpp"
#include "../serialization/split.hpp"

namespace xnet {
	namespace service {

		enum invoke_t { invoke };

		template<typename Service>
		struct remote_service_interface;

		class remote_service_implementation
			: public generic_service
		{
			friend class serialization::access;
		public:
			typedef std::function<void(const package&)> return_handler;
			typedef std::function<void(const xnet::service::call_error&)> exception_handler;

		public:
			virtual void invoke(funcid_type funcId, package args);
			template<typename Ret, typename RetHandler, typename ExcpHandler, typename... Args>
			void call(funcid_type funcId, RetHandler&& returnHandler, ExcpHandler&& excpHandler, const std::tuple<Args...>& arguments)
			{
				package arg_pack = _peer->factory()->make_package(XNET_TAGVAL(arguments), serialization::make_context(*_peer));
				_call(funcId, _make_return_handler<Ret>(returnHandler, std::is_void<Ret>()), excpHandler, std::move(arg_pack));
			}

		private:
			template<typename Ret, typename RetHandler>
			return_handler _make_return_handler(RetHandler&& handler, const std::false_type& isVoid)
			{
				{
					static_assert(std::is_convertible<RetHandler, std::function<void(Ret&&)>>::value, "RetHandler must be convertible to void(Ret)!");
					std::function<void(Ret&&)> handler2 = handler;
				}

				return [handler, this](const package& p)
				{
					Ret return_value;
					p.deserialize(XNET_TAGVAL(return_value), serialization::make_context(*_peer), package::try_next_package);
					handler(std::move(return_value));
				};
			}

			template<typename Ret, typename RetHandler, typename ExcpHandler>
			return_handler _make_return_handler(RetHandler&& handler, const std::true_type& isVoid)
			{
				{
					static_assert(std::is_convertible<RetHandler, std::function<void()>>::value, "RetHandler must be convertible to void()!");
					std::function<void()> handler2 = handler;
				}
				return [handler](const package& p)
				{
					handler();
				};
			}

			virtual void _call(funcid_type funcid, return_handler returnHandler, exception_handler excpHandler, package args) = 0;

		private:
			service_peer* _peer;
		};

		template<typename Service>
		class remote_service
		{
			friend class serialization::access;
			static_assert(is_service<Service>::value, "Service must be a service!");
		public:
			typedef remote_service_interface<Service> interface_type;
			typedef remote_service_implementation implementation_type;

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

			interface_type operator ->()
			{
				auto* impl = dynamic_cast<implementation_type*>(_service.get());
				assert(impl);
				return interface_type(impl);
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
		private:
			std::shared_ptr<generic_service> _service;
		};

	}
}


#endif