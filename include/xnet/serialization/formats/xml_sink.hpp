#pragma once
#ifndef _XNET_SERIALIZATION_FORMATS_XML_SINK_HPP
#define _XNET_SERIALIZATION_FORMATS_XML_SINK_HPP

#include <boost/lexical_cast.hpp>
#include "../../detail/rapidxml/rapidxml.hpp"
#include "../detail/primitive_operations.hpp"
#include "generic_sink.hpp"

namespace xnet {
	namespace serialization {

		class xml_sink
			: public generic_sink
		{
		public:
			typedef rapidxml::xml_node<> xml_node;
			typedef rapidxml::xml_document<> xml_document;
		public:
			xml_sink(xml_document* document, xml_node* currentNode);

			void begin_element(const char* tag) override final;
			void end_element(const char* tag) override final;

			void begin_sequence_save(const char* tag, std::size_t size) override final;
			void end_sequence_save(const char* tag) override final;

			void set_current_type(const char* type) override final;

			XNET_DETAIL_PRIMITIVE_SAVE_OPERATIONS(virtual void save, override final, out);
			XNET_DETAIL_PRIMITIVE_SAVE_OPERATIONS(virtual void save, override final, out, const char* tag);

		private:
			xml_node* _save_string(const std::string& out, const char* type);
			void _save_string(const std::string& out, const char* type, const char* tag);

			template<typename T>
			void _save(const T& out, const char* type)
			{
				_save_string(boost::lexical_cast<std::string>(out), type);
			}

			template<typename T>
			void _save(const T& out, const char* type, const char* tag)
			{
				_save_string(boost::lexical_cast<std::string>(out), type, tag);
			}

		private:
			xml_node* _currentNode;
			xml_document* _document;
		};

	}
}


#endif