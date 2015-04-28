#pragma once
#ifndef _XNET_SERVICE_CALL_ERROR_HPP
#define _XNET_SERVICE_CALL_ERROR_HPP

#include <stdexcept>

namespace xnet {
	namespace service {
		class call_error : public std::runtime_error
		{
		public:
			call_error(const std::string& msg)
				: std::runtime_error(msg)
			{
			}

			call_error(const std::string& msg, std::exception_ptr inner)
				: std::runtime_error(msg)
				, _inner(inner)
			{
			}

			std::exception_ptr inner() const
			{
				return _inner;
			}

		private:
			std::exception_ptr _inner;
		};
	}
}

#endif