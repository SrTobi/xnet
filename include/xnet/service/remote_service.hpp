#pragma once
#ifndef _XNET_SERVICE_REMOTE_SERVICE_HPP
#define _XNET_SERVICE_REMOTE_SERVICE_HPP

#include <memory>
#include "generic_service.hpp"
#include "call_error.hpp"
#include "../package.hpp"

namespace xnet {
	namespace service {
		template<typename Service>
		class remote_service
		{
			static_assert(is_service<Service>::value, "Service must be a service!");
		public:
			remote_service()
			{
			}

			remote_service(std::nullptr_t)
			{
			}

			remote_service(std::shared_ptr<Service> impl)
			{

			}

			template<typename Ret, typename... FArgs, typename... Args>
			package make_invokation(Ret(Service::*method)(FArgs...), Args&&... args)
			{
				static_assert(sizeof...(FArgs) == sizeof...(Args), "Wrong number of arguments provided!");
				static_assert(::xnet::detail::variadic_and<std::is_convertible<Args, FArgs>::value...>::value, "Provided arguments can not be converted to required types!");

				throw std::exception("Not implemented");
			}

			template<typename Ret, typename... FArgs, typename RetHandler, typename ExcpHandler, typename... Args>
			package make_call(
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

			operator bool() const
			{
				throw std::exception("Not implemented");
			}

			bool operator !() const
			{
				throw std::exception("Not implemented");
			}
		};

	}
}


#endif