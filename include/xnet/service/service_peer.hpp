#pragma once
#ifndef _XNET_SERVICE_SERVICE_PEER_HPP
#define _XNET_SERVICE_SERVICE_PEER_HPP

#include "../detail/variadic_utils.hpp"
#include "service_descriptor.hpp"
#include "call_error.hpp"
#include "../package.hpp"

namespace xnet {
	namespace service {

		class service_peer
		{
		public:
			service_peer();

			template<typename Service, typename FArgs, typename... Args>
			package make_invokation(const std::string& serviceName, void(Service::*method)(Args...), Args&&...)
			{
				static_assert(sizeof...(FArgs) == sizeof...(Args), "Wrong number of arguments provided!");
				static_assert(::xnet::detail::variadic_and<std::is_convertible<Args, FArgs>::value...>::value, "Provided arguments can not be converted to required types!");

				throw std::exception("Not implemented");
			}

			template<typename Service, typename Ret, typename... FArgs, typename RetHandler, typename ExcpHandler, typename... Args>
			package make_call(
				const std::string& serviceName,
				Ret(Service::*method)(FArgs...),
				RetHandler handler,
				ExcpHandler excpHandler,
				Args&&...)
			{

				static_assert(sizeof...(FArgs) == sizeof...(Args), "Wrong number of arguments provided!");
				static_assert(::xnet::detail::variadic_and<std::is_convertible<Args, FArgs>::value...>::value, "Provided arguments can not be converted to required types!");
				static_assert(std::is_convertible<RetHandler, std::function<void(Ret&&)>>::value, "RetHandler must be convertible to void(Ret)!");
				static_assert(std::is_convertible<ExcpHandler, std::function<void(call_error&&)>>::value, "RetHandler must be convertible to void(Ret)!");
				std::function<void(Ret&&)> func = handler;
				std::function<void(call_error&&)> func2 = excpHandler;

				throw std::exception("Not implemented");
			}

			template<typename Service>
			void add_service(const std::string& serviceName, std::shared_ptr<Service> service)
			{
				throw std::exception("Not implemented");
			}

			template<typename Service>
			std::shared_ptr<Service> get_service() const
			{
				throw std::exception("Not implemented");
			}

			void remove_service(const std::string& serviceName);

			package process_package(const package&);
		private:

		};
	}
}


#endif