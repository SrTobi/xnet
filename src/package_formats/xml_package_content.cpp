#include "xnet/detail/rapidxml/rapidxml_print.hpp"
#include "xnet/package_formats/xml_package_factory.hpp"
#include "xnet/serialization/formats/xml_sink.hpp"
#include "xnet/serialization/formats/xml_source.hpp"
#include "xnet/serialization/formats/detail/xml_serialization_names.hpp"
#include "xml_package_content.hpp"

namespace xnet {

	package_format xml_package_format("xml");


	namespace internal {
		std::shared_ptr<std::vector<unsigned char>> _make_xml_content(rapidxml::xml_node<>* node)
		{
			BOOST_ASSERT(node);

			auto result = std::make_shared<std::vector<unsigned char>>();

			rapidxml::print(std::back_inserter(*result), *node, 0);

			return result;
		}


		xml_package_content::xml_package_content(package_content_factory* factory, std::shared_ptr<rapidxml::xml_document<>> document, rapidxml::xml_node<>* node, const package& nextPkg)
			: package_content(_make_xml_content(node), nextPkg)
			, _factory(factory)
			, _document(std::move(document))
			, _node(node)
			, _next(nextPkg)
		{
			BOOST_ASSERT(node->parent() == nullptr);
			BOOST_ASSERT(rapidxml::internal::compare(_node->name(), _node->name_size(), "package", sizeof("package") - 1, true));
		}

		xml_package_content::~xml_package_content()
		{
		}

		package_content_factory* xml_package_content::factory() const
		{
			return _factory;
		}

		xnet::package_format xml_package_content::format() const
		{
			return xml_package_format;
		}

		package xml_package_content::deserialize(const deshandler_type& des) const
		{
			auto* first = _node->first_node();
			if (first)
			{
				serialization::xml_source xmlSource(first);
				des(xmlSource);
			}

			return _next;
		}

		rapidxml::xml_node<>* xml_package_content::package_node() const
		{
			return _node;
		}

	}
}