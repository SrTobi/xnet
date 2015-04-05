#include "testx/testx.hpp"
#include <xnet/serialization/formats/xml_source.hpp>
#include <xnet/serialization/formats/xml_sink.hpp>
#include <xnet/serialization/serialization.hpp>
#include <xnet/serialization/serializer.hpp>
#include <xnet/serialization/deserializer.hpp>

namespace serialization_test {

	struct SerializationTestClass
	{
		enum reference_init_t
		{
			reference_init
		};

		std::string mStringMember;

		SerializationTestClass()
		{
		}

		SerializationTestClass(reference_init_t)
			: mStringMember("a string value")
		{
		}
		
		template<typename S>
		void serialize(S& s)
		{
			XNET_CURRENT_TYPE(s, *this, SerializationTestClass);
			s & mStringMember;
		}

		bool operator ==(const SerializationTestClass& right) const
		{
			return mStringMember == right.mStringMember;
		}
	};

	template<typename Ch>
	std::basic_ostream<Ch>& operator <<(std::basic_ostream<Ch>& s, const SerializationTestClass& out)
	{
		return s;
	}

	const SerializationTestClass reference_instance(SerializationTestClass::reference_init);



	template<typename Sink>
	void test_sink_format(Sink& sink)
	{
		xnet::serialization::serializer<Sink> s(sink);
		s << reference_instance;
	}

	template<typename Source>
	void test_source_format(Source& source)
	{
		xnet::serialization::deserializer<Source> s(source);
		SerializationTestClass c;
		s >> c;
		BOOST_CHECK_EQUAL(c, reference_instance);
	}



	TESTX_AUTO_TEST_CASE(test_xml_format)
	{

		rapidxml::xml_document<> doc;
		auto root = doc.allocate_node(rapidxml::node_element, "package");
		doc.append_node(root);

		{
			typedef xnet::serialization::xml_sink xmls;
			xmls sink(&doc, root);
			test_sink_format(sink);
		}

		{
			typedef xnet::serialization::xml_source xmls;
			xmls source(root->first_node());
			test_source_format(source);
		}
	}


}