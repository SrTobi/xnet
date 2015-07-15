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
		public:
			typedef std::function<void(const xnet::service::call_error&)> exception_handler;
			typedef std::function<void()> chat_return_handler;

		public:
			virtual void invoke(...);
			virtual void call(...);
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
			std::shared_ptr<generic_service> _service;
		};

	}
}


#endif