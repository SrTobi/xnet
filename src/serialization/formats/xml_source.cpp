#include <boost/assert.hpp>
#include <boost/locale.hpp>
#include <boost/locale/utf.hpp>
#include "xnet/serialization/detail/serialization_check.hpp"
#include "xnet/serialization/formats/xml_source.hpp"
#include "xnet/serialization/formats/detail/xml_serialization_names.hpp"

namespace xnet {
	namespace serialization {

		namespace detail {
			const char* ELEMENT_NAME = "element";
			const char* PRIMITIVE_NAME = "primitive";
			const char* TAG_NAME = "tag";
			const char* TYPE_NAME = "type";
		}

		bool equals(const char* left, const char* right)
		{
			while (*left == *right && *left != '\0')
			{
				++left; ++right;
			}

			return *left == *right;
		}



		xml_source::xml_source(xml_node* node)
			: _currentNode(node)
		{
			BOOST_ASSERT(node);
			_rootNode = node->parent();
			_elementNode = nullptr;
		}

		void xml_source::begin_element(const char* tag)
		{
			XNET_SERIALIZATION_CHECK(_currentNode && equals(_currentNode->name(), detail::ELEMENT_NAME), "expected element!");
			if (tag)
			{
				auto* attr = _currentNode->first_attribute(detail::TAG_NAME);
				XNET_SERIALIZATION_CHECK(attr, "expected tag attribute");
				XNET_SERIALIZATION_CHECK(equals(attr->value(), tag), std::string("wrong tag! expected tag '") + tag + "'");
			}

			_elementNode = _currentNode;
			_currentNode = _elementNode->first_node();
		}

		void xml_source::end_element(const char* tag)
		{
			XNET_SERIALIZATION_CHECK(_elementNode, "no element to end!");
			_currentNode = _elementNode;
			_elementNode = _elementNode->parent();

			if (_elementNode == _rootNode)
				_elementNode = nullptr;
		}

		bool xml_source::begin_array(const char* tag, std::size_t& size)
		{
			return true;
		}

		void xml_source::end_array(const char* tag)
		{

		}

		void xml_source::check_current_type(const char* type) const
		{
			XNET_SERIALIZATION_CHECK(_elementNode, "no current element to check!");
			auto* attr = _elementNode->first_attribute(detail::TYPE_NAME);
			XNET_SERIALIZATION_CHECK(attr, "no type information for current element!");
			XNET_SERIALIZATION_CHECK(equals(attr->value(), type),
				std::string("wrong element type! expected '") + type + "' but found '" + attr->value() + "'");
		}

		std::string xml_source::_load(const char* type)
		{
			XNET_SERIALIZATION_CHECK(_currentNode && equals(_currentNode->name(), detail::PRIMITIVE_NAME), "no current primitive");

			// type check
			{
				auto* attr = _currentNode->first_attribute(detail::TYPE_NAME);
				XNET_SERIALIZATION_CHECK(attr, "no type information fot current primitive!");
				XNET_SERIALIZATION_CHECK(equals(attr->value(), type),
					std::string("wrong primitive type! expected '") + type + "' but found '" + attr->value() + "'");
			}

			auto* cur = _currentNode;
			_currentNode = _currentNode->next_sibling();
			return std::string(cur->value(), cur->value_size());
		}

		std::string xml_source::_load(const char* type, const char* tag)
		{
			auto* curNode = _currentNode;
			auto value = _load(type);

			{
				auto* attr = curNode->first_attribute(detail::TAG_NAME);
				XNET_SERIALIZATION_CHECK(attr, "expected tag attribute");
				XNET_SERIALIZATION_CHECK(equals(attr->value(), tag), std::string("wrong tag! expected tag '") + tag + "'");
			}

			return value;
		}

#define MAKE_LOAD(type, typeName, ...) \
	void xml_source::load(type& in) { auto value = _load(typeName); __VA_ARGS__; } \
	void xml_source::load(type& in, const char* tag) { auto value = _load(typeName, tag); __VA_ARGS__;}

#define MAKE_NUMERIC_LOAD(type) \
	MAKE_LOAD(type, #type, in = boost::lexical_cast<type>(value);)

#define MAKE_LITERAL_LOAD(type) \
	MAKE_LOAD(type, #type, in = boost::locale::conv::utf_to_utf<type::value_type>(value);)

		MAKE_NUMERIC_LOAD(bool)
		MAKE_NUMERIC_LOAD(signed char)
		MAKE_NUMERIC_LOAD(unsigned char)
		//MAKE_NUMERIC_LOAD(wchar_t)
		MAKE_NUMERIC_LOAD(signed short)
		MAKE_NUMERIC_LOAD(unsigned short)
		MAKE_NUMERIC_LOAD(signed int)
		MAKE_NUMERIC_LOAD(unsigned int)
		MAKE_NUMERIC_LOAD(signed long)
		MAKE_NUMERIC_LOAD(unsigned long)
		MAKE_NUMERIC_LOAD(float)
		MAKE_NUMERIC_LOAD(double)
		MAKE_NUMERIC_LOAD(long double)

		MAKE_LOAD(wchar_t, "wchar_t", 
			auto tmp = boost::locale::conv::utf_to_utf<wchar_t>(value);
			in = tmp[0];
		)

		MAKE_LOAD(std::string, "std::string",
			in = value;
		)
		MAKE_LITERAL_LOAD(std::wstring);
		MAKE_LITERAL_LOAD(std::u16string);
		MAKE_LITERAL_LOAD(std::u32string);

	}
}