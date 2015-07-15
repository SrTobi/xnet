#pragma once
#ifndef _REMOTE_LOGIN_SERVICE_HPP
#define _REMOTE_LOGIN_SERVICE_HPP

#include <xnet/service/remote_service.hpp>
#include "service-definition.hpp"

namespace xnet {
	namespace service {

		template<>
		class remote_service_interface <LoginService>
		{
		public:
			typedef std::function<void(const xnet::service::call_error&)> exception_handler;
			typedef std::function<void(remote_service<ChatService>)> login_return_handler;

		public:
			remote_service_interface(remote_service_implementation* impl);
			~remote_service_interface();

			void login(std::string msg, xnet::service::invoke_t);
			void login(std::string password, const login_return_handler& ret_handler, const exception_handler& excp_handler);

			remote_service_interface* operator ->();
		private:
			remote_service_interface* _impl;
		};
	}
}

#endif