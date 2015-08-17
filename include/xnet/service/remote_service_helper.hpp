#pragma once
#ifndef _REMOTE_SERVICE_HELPER_HPP
#define _REMOTE_SERVICE_HELPER_HPP

#include "../package.hpp"
#include "../package_factory.hpp"


namespace xnet {
	namespace service {
		
		template<typename Ret, typename RetHandler>
		returnid_type _make_return_hanler(RetHandler&& handler, const std::false_type& isVoid)
		{
			{
				static_assert(std::is_convertible<RetHandler, std::function<void(Ret&&)>>::value, "RetHandler must be convertible to void(Ret)!");
				std::function<void(Ret&&)> handler2 = handler;
			}
			auto rh = [handler, this](const package& p)
			{
				Ret return_value;
				p.deserialize(XNET_TAGVAL(return_value), serialization::make_context(*this), package::try_next_package);
				handler(std::move(return_value));
			};

			return _create_return_id(std::move(rh), excpHandler);
		}

		template<typename Ret, typename RetHandler>
		returnid_type _make_return_id(RetHandler&& handler, ExcpHandler&& excpHandler, const std::true_type& isVoid)
		{
			{
				static_assert(std::is_convertible<RetHandler, std::function<void()>>::value, "RetHandler must be convertible to void()!");
				std::function<void()> handler2 = handler;
			}
			auto rh = [handler](const package& p)
			{
				handler();
			};

			return _create_return_id(std::move(rh), excpHandler);
		}
	}
}


#endif