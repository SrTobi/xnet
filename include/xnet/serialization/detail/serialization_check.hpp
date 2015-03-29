#pragma once
#ifndef _XNET_DETAIL_SERIALIZATION_CHECK_HPP
#define _XNET_DETAIL_SERIALIZATION_CHECK_HPP

#include "../serialization_error.hpp"

#define XNET_SERIALIZATION_CHECK(cond, what) \
	{ if (!(cond)) { throw ::xnet::serialization_error(what); }}

#endif