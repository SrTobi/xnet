#include "xnet/detail/rapidxml/rapidxml_print.hpp"
#include "xnet/package_formats/xml_package_factory.hpp"
#include "xnet/serialization/formats/xml_sink.hpp"
#include "xml_package_content.hpp"

namespace xnet {

	xml_package_factory::xml_package_factory()
		: package_content_factory(xml_package_format)
	{
	}

	xml_package_factory::~xml_package_factory()
	{
	}

	package xml_package_factory::construct(const constr_type& constr, const package& nextPkg)
	{
		using namespace internal;
		
		auto document = std::make_shared<rapidxml::xml_document<>>();

		auto* node = document->allocate_node(rapidxml::node_element, "package");

		serialization::xml_sink sink(document.get(), node);
		constr(sink);

		return package::make_package<xml_package_content>(this, std::move(document), node, nextPkg);
	}

	package xml_package_factory::construct(std::vector<unsigned char> data)
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

}