#pragma once
#ifndef _RemoteChatService_HPP
#define _RemoteChatService_HPP

#include <xnet/service/remote_service.hpp>
#include "service-definition.hpp"

namespace xnet {
	namespace service {

		template<>
		class remote_service_interface<ChatService>
		{
		public:
			typedef std::function<void(const xnet::service::call_error&)> exception_handler;
			typedef std::function<void(const std::string&)> chat_return_handler;

		public:
			remote_service_interface(remote_service_implementation* impl);
			~remote_service_interface();

			void chat(const std::string& msg, xnet::service::invoke_t);
			void chat(const std::string& msg,  chat_return_handler ret_handler, exception_handler excp_handler);

			remote_service_interface* operator ->();
		private:
			remote_service_implementation* _impl;
		};
	}
}


#endif