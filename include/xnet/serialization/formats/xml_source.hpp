#pragma once
#ifndef _XNET_SERIALIZATION_FORMATS_XML_SOURCE_HPP
#define _XNET_SERIALIZATION_FORMATS_XML_SOURCE_HPP

#include <boost/lexical_cast.hpp>
#include "../../detail/rapidxml/rapidxml.hpp"
#include "../detail/primitive_operations.hpp"
#include "generic_source.hpp"

namespace xnet {
	namespace serialization {

		class xml_source: public generic_source
		{
		public:
			typedef rapidxml::xml_node<> xml_node;

			xml_source(xml_node* node);

			void begin_element(const char* tag) override final;
			void end_element(const char* tag) override final;

			void begin_sequence_load(const char* tag, std::size_t& size) override final;
			void end_sequence_load(const char* tag) override final;

			void check_current_type(const char* tag) const override final;

			XNET_DETAIL_PRIMITIVE_LOAD_OPERATIONS(virtual void load, override final, in);
			XNET_DETAIL_PRIMITIVE_LOAD_OPERATIONS(virtual void load, override final, in, const char* tag);
		private:
			std::string _load(const char* type);
			std::string _load(const char* type, const char* tag);

		private:
			xml_node* _rootNode;
			xml_node* _elementNode;
			xml_node* _currentNode;
		};

	}
}


#endif