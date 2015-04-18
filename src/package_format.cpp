#include <boost/assert.hpp>
#include "xnet/package_format.hpp"


namespace xnet {
	extern package_format raw_package_format("RAW");

	/************************************** package_format **************************************/
	package_format::package_format(const char format[4])
		: _format({ format[0], format[1], format[2], '\0' })
	{
		BOOST_ASSERT(format[3] == '\0');
	}

	package_format::package_format(const package_format& other)
		: _formatId(other._formatId)
	{
	}

	const char* package_format::name() const
	{
		return _format.data();
	}

	package_format::id_type package_format::id() const
	{
		return _formatId;
	}

	bool package_format::operator==(const package_format& right) const
	{
		return _formatId == right._formatId;
	}

	bool package_format::operator!=(const package_format& right) const
	{
		return !(*this == right);
	}
}