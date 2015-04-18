#pragma once
#ifndef _XNET_INTERNAL_XML_PACKAGE_CONTENT_HPP
#define _XNET_INTERNAL_XML_PACKAGE_CONTENT_HPP

#include <deque>
#include "xnet/detail/rapidxml/rapidxml.hpp"
#include "xnet/package.hpp"

namespace xnet {
	namespace internal {

		class xml_package_content: public package_content
		{
			friend class xml_package_factory;
		public:
			xml_package_content(
				package_content_factory* factory,
				std::shared_ptr<rapidxml::xml_document<>> document,
				rapidxml::xml_node<>* node,
				const package& nextPkg);

			virtual ~xml_package_content();

			virtual package_content_factory* factory() const final override;
			virtual package_format format() const final override;
			virtual package deserialize(const deshandler_type& des) const final override ;

			rapidxml::xml_node<>* package_node() const;

		private:
			std::shared_ptr<rapidxml::xml_document<>> _document;
			rapidxml::xml_node<>* _node;
			package_content_factory* _factory;
			package _next;
		};
	}
}

#endif