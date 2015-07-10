#pragma once
#ifndef _XNET_SERVICE_SERVICE_HPP
#define _XNET_SERVICE_SERVICE_HPP

#include <memory>
#include <assert.h>

namespace xnet {
	namespace service {
		namespace internal {
			class remote_service_backend;
		}
		typedef uint16_t funcid_type;
		typedef uint32_t returnid_type;
		typedef uint16_t serviceid_type;
		class service_peer;
		class generic_service;
		class generic_service_descriptor;

		class generic_service
			: public std::enable_shared_from_this<generic_service>
		{
			friend class service_peer;
			template<typename Service>
			friend class remote_service;
			friend class internal::remote_service_backend;
		public:
			generic_service();
			virtual ~generic_service();

		protected:
			virtual const generic_service_descriptor& _descriptor() const = 0;
			service_peer call_origin() const;
		private:
			generic_service(service_peer* peer);

		private:
			const bool _local = true;
			service_peer* _peer;
		};

		template<typename Service>
		class abstract_service
			: public generic_service
		{
			friend class service_peer;
		protected:
			virtual const generic_service_descriptor& _descriptor() const override final;
		};

		template<typename Service>
		struct is_service
			: public std::is_base_of<generic_service, Service>
		{
		};

#define XNET_DECLARE_SERVICE(name) struct name;
#define XNET_SERVICE(name) struct name : public ::xnet::service::abstract_service<name>
	}
}


#endif