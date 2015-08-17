#pragma once
#ifndef _XNET_SERVICE_SERVICE_DESCRIPTOR_HPP
#define _XNET_SERVICE_SERVICE_DESCRIPTOR_HPP

#include <exception>
#include "../package.hpp"
#include "generic_service.hpp"

namespace xnet {

	namespace service {
		class service_method_descriptor
		{
		public:
			service_method_descriptor(const std::string& name, funcid_type id);
			~service_method_descriptor();

			funcid_type id() const;
			const std::string& name() const;

			virtual void invoke(service_peer& peer, generic_service& service, const package& args) const = 0;
			virtual package call(service_peer& peer, generic_service& service, const package& args) const = 0;
		private:
			const std::string& _name;
			const funcid_type _id;
		};

		class service_descriptor
		{
		public:
			typedef std::shared_ptr<service_method_descriptor> method_descriptor_type;
		public:
			service_descriptor(const std::string& name, const std::string& checksum, std::initializer_list<method_descriptor_type> descs);
			~service_descriptor();

			const std::string& name() const;
			const std::string& checksum() const;
			const std::vector<std::shared_ptr<service_method_descriptor>>& methods() const;

			void invoke(service_peer& peer, generic_service& service, funcid_type id, const package& args) const;
			package call(service_peer& peer, generic_service& service, funcid_type id, const package& args) const;

		private:
			const std::string _name;
			const std::string _checksum;
			const std::vector<method_descriptor_type> _methods;
		};

		namespace detail {
			template<typename Service>
			struct service_descriptor_getter
			{
				static_assert(is_service<Service>::value, "Service must be a service!");
				static const service_descriptor& get();
			};
		}

		template<typename Service>
		const service_descriptor& get_descriptor()
		{
			static_assert(is_service<Service>::value, "Service must be a service!");
			return detail::service_descriptor_getter<Service>::get();
		}
	}
}


#endif