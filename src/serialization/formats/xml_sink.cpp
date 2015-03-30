#include <boost/assert.hpp>
#include <boost/locale.hpp>
#include <boost/locale/utf.hpp>
#include "xnet/serialization/formats/xml_sink.hpp"
#include "xnet/serialization/formats/detail/xml_serialization_names.hpp"

namespace xnet {
	namespace serialization {

		xml_sink::xml_sink(xml_document* document, xml_node* currentNode)
			: _document(document)
			, _currentNode(currentNode)
		{
			BOOST_ASSERT(document);
			BOOST_ASSERT(currentNode);
		}

		void xml_sink::begin_element(const char* tag)
		{
			auto* newNode = _document->allocate_node(rapidxml::node_element, detail::ELEMENT_NAME);
			_currentNode->append_node(newNode);

			if (tag)
			{
				auto* attr = _document->allocate_attribute(detail::TAG_NAME, tag);
				newNode->append_attribute(attr);
			}
			_currentNode = newNode;
		}

		void xml_sink::end_element(const char* tag)
		{
			{
				auto* attr = _currentNode->first_attribute(detail::TAG_NAME);
				BOOST_ASSERT(!attr == !tag);
				if (tag)
				{
					BOOST_ASSERT(attr->value() == tag);
				}
				BOOST_ASSERT(_currentNode->name() == detail::ELEMENT_NAME);
				BOOST_ASSERT(_currentNode->parent());
			}
			_currentNode = _currentNode->parent();
		}

		void xml_sink::begin_array(const char* tag, std::size_t size)
		{

		}

		void xml_sink::end_array(const char* tag)
		{

		}

		void xml_sink::set_current_type(const char* type)
		{
			BOOST_ASSERT(_currentNode->first_attribute(detail::TYPE_NAME) == nullptr);
			auto* attr = _document->allocate_attribute(detail::TYPE_NAME, type);
			_currentNode->prepend_attribute(attr);
		}

		xml_sink::xml_node* xml_sink::_save_string(const std::string& out, const char* type)
		{
			auto* text = _document->allocate_string(out.data(), out.size());
			auto* newNode = _document->allocate_node(rapidxml::node_element, detail::PRIMITIVE_NAME, text, 0, out.size());

			auto* attr = _document->allocate_attribute(detail::TYPE_NAME, type);
			newNode->append_attribute(attr);

			_currentNode->append_node(newNode);
			return newNode;
		}

		void xml_sink::_save_string(const std::string& out, const char* type, const char* tag)
		{
			auto* node = _save_string(out, type);
			auto* attr = _document->allocate_attribute(detail::TAG_NAME, tag);
			node->append_attribute(attr);
		}

#define MAKE_SAVE(type, typeName, ...) \
	void xml_sink::save(type out) { __VA_ARGS__; _save(value, typeName);} \
	void xml_sink::save(type out, const char* tag) { __VA_ARGS__; _save(value, typeName, tag);}

#define MAKE_NUMERIC_SAVE(type) \
	MAKE_SAVE(type, #type, const auto value = boost::lexical_cast<std::string>(out);)

#define MAKE_LITERAL_SAVE(type, typeName) \
	MAKE_SAVE(type, typeName, const auto value = boost::locale::conv::utf_to_utf<char>(out);)

		MAKE_NUMERIC_SAVE(bool)
		MAKE_NUMERIC_SAVE(char)
		MAKE_SAVE(unsigned char, "byte",
			unsigned short value = out;
		)
		//MAKE_NUMERIC_SAVE(wchar_t)
		MAKE_NUMERIC_SAVE(short)
		MAKE_NUMERIC_SAVE(unsigned short)
		MAKE_NUMERIC_SAVE(int)
		MAKE_NUMERIC_SAVE(unsigned int)
		MAKE_NUMERIC_SAVE(long)
		MAKE_NUMERIC_SAVE(unsigned long)
		MAKE_NUMERIC_SAVE(float)
		MAKE_NUMERIC_SAVE(double)
		MAKE_NUMERIC_SAVE(long double)

		MAKE_SAVE(wchar_t, "wchar_t",
			auto value = boost::locale::conv::utf_to_utf<char>(std::wstring(1, out));
		)

		MAKE_SAVE(const std::string&, "std::string",
			const auto& value = out;
		)
		MAKE_LITERAL_SAVE(const std::wstring&, "std::wstring");
		MAKE_LITERAL_SAVE(const std::u16string&, "std::16string");
		MAKE_LITERAL_SAVE(const std::u32string&, "std::32string");
		MAKE_LITERAL_SAVE(const char*, "char*");
		MAKE_LITERAL_SAVE(const wchar_t*, "wchar_t*");
		MAKE_LITERAL_SAVE(const char16_t*, "char16_t*");
		MAKE_LITERAL_SAVE(const char32_t*, "char32_t*");

	}
}