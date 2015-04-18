#pragma once
#ifndef _XNET_XML_PACKAGE_HPP
#define _XNET_XML_PACKAGE_HPP

#include "../detail/rapidxml/rapidxml.hpp"
#include "../package_factory.hpp"

namespace xnet {

	extern package_format xml_package_format;

	class xml_package_factory: public package_content_factory
	{
	public:
		xml_package_factory();
		virtual ~xml_package_factory();

		virtual package construct(const constr_type& constr, const package& nextPkg) final override;
		virtual package construct(std::vector<unsigned char> data) final override;

	};
}


#endif