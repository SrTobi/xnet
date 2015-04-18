#pragma once
#ifndef _XNET_PACKAGE_FORMAT_HPP
#define _XNET_PACKAGE_FORMAT_HPP

#include <array>
#include <memory>
#include <stddef.h>

namespace xnet {

	class package_format
	{
	public:
		typedef uint32_t id_type;
	public:
		package_format(const char format[4]);
		package_format(const package_format& other);

		const char* name() const;
		id_type id() const;

		bool operator ==(const package_format& right) const;
		bool operator !=(const package_format& right) const;

	private:
		union {
			id_type				_formatId;
			std::array<char, 4> _format;

			static_assert(sizeof(decltype(_formatId)) == sizeof(decltype(_format)), "_formatId should equal _format!");
		};
	};

	extern package_format raw_package_format;
}

#endif