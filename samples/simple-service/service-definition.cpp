#include "RemoteChatService.hpp"
#include "RemoteLoginService.hpp"

namespace xnet {
	namespace service {


		enum class _LoginService_function_ids : funcid_type
		{
			func_login = 1
		};

		remote_service_interface<LoginService>::remote_service_interface(remote_service_implementation* impl)
			: _impl(impl)
		{
		}

		remote_service_interface<LoginService>::~remote_service_interface()
		{
		}

		void remote_service_interface<LoginService>::login(std::string password, xnet::service::invoke_t)
		{
		}

		void remote_service_interface<LoginService>::login(std::string password, login_return_handler ret_handler, exception_handler excp_handler)
		{
			typedef remote_service<ChatService> return_type;
			auto args = std::forward_as_tuple(password);
			funcid_type funcid = funcid_type(_LoginService_function_ids::func_login);
			_impl->call<return_type>(funcid, ret_handler, excp_handler, args);
		}

		remote_service_interface<LoginService>* remote_service_interface<LoginService>::operator ->()
		{
			return this;
		}

		namespace detail {

			const service_descriptor& service_descriptor_getter<LoginService>::get()
			{
				static struct _ : public service_descriptor
				{
					typedef std::tuple<std::string> _LoginService_function_login_arguments;

					_()
						: service_descriptor("LoginService", "12345", {})
					{
					}

					virtual void invoke(service_peer& peer, generic_service& service, funcid_type id, const package& args) const override
					{
						auto& s = dynamic_cast<LoginService&>(service);
						switch (id)
						{
						case funcid_type(_LoginService_function_ids::func_login):
							{
								_LoginService_function_login_arguments arguments;
								args.deserialize(XNET_TAGVAL(arguments), xnet::serialization::make_context(peer), xnet::package::try_next_package);
								s.login(std::get<0>(arguments));
							}break;
						default:
							throw std::runtime_error("Function does not exist");
						}
					}

					virtual package call(service_peer& peer, generic_service& service, funcid_type id, const package& args) const override
					{
						throw std::logic_error("The method or operation is not implemented.");
					}

				} desc;
				return desc;
			}
		}
	}
}


namespace xnet {
	namespace service {

		enum class _ChatService_function_ids : funcid_type
		{
			func_chat = 1
		};

		remote_service_interface<ChatService>::remote_service_interface(remote_service_implementation* impl)
			: _impl(impl)
		{
		}

		remote_service_interface<ChatService>::~remote_service_interface()
		{
		}

		void remote_service_interface<ChatService>::chat(const std::string& msg, xnet::service::invoke_t)
		{
		}

		void remote_service_interface<ChatService>::chat(const std::string& msg, chat_return_handler ret_handler, exception_handler excp_handler)
		{
			typedef std::string return_type;
			auto args = std::forward_as_tuple(msg);
			funcid_type funcid = funcid_type(_ChatService_function_ids::func_chat);
			_impl->call<return_type>(funcid, ret_handler, excp_handler, args);
		}

		remote_service_interface<ChatService>* remote_service_interface<ChatService>::operator ->()
		{
			return this;
		}

		namespace detail {

			const service_descriptor& service_descriptor_getter<ChatService>::get()
			{
				static struct _ : public service_descriptor
				{
					typedef std::tuple<std::string> _LoginService_function_login_arguments;

					_()
						: service_descriptor("ChatService", "54321", {})
					{
					}

					virtual void invoke(service_peer& peer, generic_service& service, funcid_type id, const package& args) const override
					{
						auto& s = dynamic_cast<ChatService&>(service);
						switch (id)
						{
						case funcid_type(_ChatService_function_ids::func_chat):
							{
								_LoginService_function_login_arguments arguments;
								args.deserialize(XNET_TAGVAL(arguments), xnet::serialization::make_context(peer), xnet::package::try_next_package);
								s.chat(std::get<0>(arguments));
							}break;
						default:
							throw std::runtime_error("Function does not exist");
						}
					}

					virtual package call(service_peer& peer, generic_service& service, funcid_type id, const package& args) const override
					{
						throw std::logic_error("The method or operation is not implemented.");
					}

				} desc;
				return desc;
			}
		}
	}
}
