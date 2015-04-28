#pragma once
#ifndef _XNET_SERVICE_PROTOCOL_SERVICE_PROTOCOL_HPP
#define _XNET_SERVICE_PROTOCOL_SERVICE_PROTOCOL_HPP

#include <tuple>
#include <string>
#include "../../serialization/types/enum.hpp"
#include "../../serialization/types/tuple.hpp"
#include "../service_descriptor.hpp"

namespace xnet {
	namespace service {
		namespace protocol {
			enum class invokation_type : funcid_type
			{
				return_invokation = 1,
				return_exception = 2,
				static_call = 3,
				dynamic_call = 4,
				static_invokation = 5,
				dynamic_invokation = 6,
				static_call_resolved = 7,
				dynamic_call_resolved = 8
			};
			XNET_SERIALIZATION_ENUM(invokation_type);

			template<typename... Args>
			struct static_invokation
			{
				std::string service_name;
				std::string service_checksum;
				funcid_type func_id;
				std::tuple<Args...> arguments;

				template<typename S>
				void serialize(S& s)
				{
					s & service_name & service_checksum & func_id & arguments;
				}
			};

			template<typename... Args>
			struct static_call
			{
				std::string service_name;
				std::string service_checksum;
				funcid_type func_id;
				returnid_type return_id;
				std::tuple<Args...> arguments;

				template<typename S>
				void serialize(S& s)
				{
					s & service_name & service_checksum & func_id & return_id & arguments;
				}
			};

			template<typename... Args>
			struct dynamic_invokation
			{
				serviceid_type service_id;
				funcid_type func_id;
				std::tuple<Args...> arguments;

				template<typename S>
				void serialize(S& s)
				{
					s & service_id & func_id & arguments;
				}
			};

			template<typename... Args>
			struct dynamic_call
			{
				serviceid_type service_id;
				funcid_type func_id;
				returnid_type return_id;
				std::tuple<Args...> arguments;

				template<typename S>
				void serialize(S& s)
				{
					s & service_id & func_id & return_id & arguments;
				}
			};

			template<typename Ret>
			struct return_invokation
			{
				returnid_type return_id;
				Ret return_value;

				template<typename S>
				void serialize(S& s)
				{
					s & return_id & return_value;
				}
			};

			struct return_exception
			{
				returnid_type return_id;
				std::string message;

				template<typename S>
				void serialize(S& s)
				{
					s & return_id & message;
				}
			};
		}
	}
}

#endif