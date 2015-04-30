#pragma once
#ifndef _XNET_SERVICE_PROTOCOL_SERVICE_PROTOCOL_HPP
#define _XNET_SERVICE_PROTOCOL_SERVICE_PROTOCOL_HPP

#include <tuple>
#include <string>
#include <boost/variant.hpp>
#include "xnet/serialization/serialization.hpp"
#include "xnet/serialization/types/enum.hpp"
#include "xnet/serialization/types/tuple.hpp"
#include "xnet/serialization/types/variant.hpp"
#include "xnet/service/service_descriptor.hpp"

namespace xnet {
	namespace service {
		namespace protocol {
			struct static_invokation
			{
				std::string service_name;
				std::string service_checksum;
				funcid_type func_id;

				template<typename S>
				void serialize(S& s)
				{
					s & XNET_TAGVAL(service_name) & XNET_TAGVAL(service_checksum) & XNET_TAGVAL(func_id);
				}
			};

			struct static_call
			{
				std::string service_name;
				std::string service_checksum;
				funcid_type func_id;
				returnid_type return_id;

				template<typename S>
				void serialize(S& s)
				{
					s & XNET_TAGVAL(service_name) & XNET_TAGVAL(service_checksum) & XNET_TAGVAL(func_id) & XNET_TAGVAL(return_id);
				}
			};

			struct dynamic_invokation
			{
				serviceid_type service_id;
				funcid_type func_id;

				template<typename S>
				void serialize(S& s)
				{
					s & XNET_TAGVAL(service_id) & XNET_TAGVAL(func_id);
				}
			};

			struct dynamic_call
			{
				serviceid_type service_id;
				funcid_type func_id;
				returnid_type return_id;

				template<typename S>
				void serialize(S& s)
				{
					s & XNET_TAGVAL(service_id) & XNET_TAGVAL(func_id) & XNET_TAGVAL(return_id);
				}
			};

			struct return_invokation
			{
				returnid_type return_id;

				template<typename S>
				void serialize(S& s)
				{
					s & XNET_TAGVAL(return_id);
				}
			};

			struct return_exception
			{
				returnid_type return_id;
				std::string message;

				template<typename S>
				void serialize(S& s)
				{
					s & XNET_TAGVAL(return_id) & XNET_TAGVAL(message);
				}
			};

			typedef boost::variant<static_invokation, static_call, dynamic_invokation, dynamic_call, return_invokation, return_exception> header;
		}
	}
}

#endif