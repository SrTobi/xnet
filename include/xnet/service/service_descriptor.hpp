#pragma once
#ifndef _XNET_SERVICE_SERVICE_DESCRIPTOR_HPP
#define _XNET_SERVICE_SERVICE_DESCRIPTOR_HPP

#include "generic_service.hpp"

namespace xnet {
	namespace service {

		/*template<typename Service>
		class service_method_descriptor
		{
		public:
			service_method_descriptor()
		};*/

		template<typename Service>
		class service_descriptor
		{
			static_assert(is_service<Service>::value, "Service must be a service!");
		public:
			template<typename InitHandler>
			service_descriptor(const std::string& name, const InitHandler& handler)
			{
				handler(*this);
			}

			template<typename Ret, typename... Args>
			service_descriptor& add_method(const std::string& name, Ret (Service::*method)(Args...))
			{
				throw std::exception("Not implemented!");
			}

			template<typename Ret, typename... Args>
			funcid_type resolve_method(Ret(Service::*method)(Args...)) const
			{
				throw std::exception("Not implemented!");
			}

			const std::string& checksum() const
			{
				return _checksum;
			}

		private:
			std::string _checksum;
		};


		namespace detail {
			template<typename Service>
			struct service_descriptor_getter
			{
				static_assert(is_service<Service>::value, "Service must be a service!");
				inline static const service_descriptor<Service>& get();
			};
		}

		template<typename Service>
		const service_descriptor<Service>& get_descriptor()
		{
			static_assert(is_service<Service>::value, "Service must be a service!");
			return detail::service_descriptor_getter<Service>::get();
		}

#define XNET_IMPLEMENT_SERVICE_DESCRIPTOR_NAMED(_service, _name, _desk)	\
		void _Xnet_init_service_##_service##_descriptor(::xnet::service::service_descriptor<_service>& _desk); \
		namespace xnet_detail { const ::xnet::service::service_descriptor<_service> _Xnet_##_service##_descriptor(_name, \
		[](::xnet::service::service_descriptor<_service>& desk){ _Xnet_init_service_##_service##_descriptor(desk); }); }\
		namespace xnet {namespace service { const service_descriptor<_service>& detail::service_descriptor_getter<_service>::get() { return ::xnet_detail::_Xnet_##_service##_descriptor;}}} \
		void _Xnet_init_service_##_service##_descriptor(::xnet::service::service_descriptor<_service>& _desk)

#define XNET_IMPLEMENT_SERVICE_DESCRIPTOR(_service, _desk)	\
			XNET_IMPLEMENT_SERVICE_DESCRIPTOR_NAMED(_service, #_service, _desk)
	}
}


#endif