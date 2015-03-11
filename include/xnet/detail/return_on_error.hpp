#pragma once
#ifndef _XNET_RETURN_ON_ERROR_HPP
#define _XNET_RETURN_ON_ERROR_HPP

#define XNET_RETURN_ERROR(ec)			if(ec) { return (ec); }
#define XNET_RETURN_ON_ERROR(ec, ret)	if(ec) { return (ret); }

#endif