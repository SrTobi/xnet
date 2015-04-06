#include "testx/testx.hpp"
#include <xnet/serialization/formats/xml_source.hpp>
#include <xnet/serialization/formats/xml_sink.hpp>
#include <xnet/serialization/serialization.hpp>
#include <xnet/serialization/serializer.hpp>
#include <xnet/serialization/deserializer.hpp>
#include <xnet/serialization/types/forward_list.hpp>
#include <xnet/serialization/types/list.hpp>
#include <xnet/serialization/types/unordered_set.hpp>
#include <xnet/serialization/types/unordered_map.hpp>
#include <xnet/serialization/types/map.hpp>
#include <xnet/serialization/types/set.hpp>
#include <xnet/serialization/types/tuple.hpp>
#include <xnet/serialization/types/array.hpp>

namespace serialization_test {

	struct SerializationTestClass
	{
		enum reference_init_t
		{
			reference_init
		};

		bool mBoolMember = false;
		int mIntMember = 0;
		std::string mStringMember;
		std::pair<int, int> mPairMember;
		std::tuple<char, unsigned char, short, unsigned short> mTupleMember;
		std::forward_list<std::string> mForwardListMember;
		std::list<unsigned int> mListMember;
		std::map<std::string, unsigned int> mMapMember;
		std::set<std::string> mSetMember;
		std::unordered_set<std::string> mUnorderedSetMember;
		std::unordered_map<int, std::string> mUnorderedMapMember;
		std::array<int, 5> mArrayMember;


		SerializationTestClass()
		{
		}

		SerializationTestClass(reference_init_t)
			: mBoolMember(true)
			, mIntMember(12345)
			, mStringMember("a string value")
			, mPairMember(234, 765)
			, mTupleMember('c', 18, 19, 20)
			, mForwardListMember({ "test1", "test2", "test3" })
			, mListMember({ 100, 101, 102, 103, 104 })
			, mMapMember({ { "test1", 88 }, { "test2", 22 } })
			, mSetMember({ "x", "z", "y" })
			, mUnorderedSetMember({ "a", "b", "c" })
			, mUnorderedMapMember({ { 0, "a" }, { 1, "b" } })
			, mArrayMember({1, 2, 3, 4, 5})
		{
		}
		
		template<typename S>
		void serialize(S& s)
		{
			XNET_CURRENT_TYPE(s, *this, SerializationTestClass);
			s & mBoolMember & mIntMember & mStringMember & mPairMember & mTupleMember & mForwardListMember
				& mListMember & mMapMember & mSetMember & mUnorderedSetMember & mUnorderedMapMember & mArrayMember;
		}

		bool operator ==(const SerializationTestClass& right) const
		{
			return mBoolMember == mBoolMember
				&& mIntMember == mIntMember
				&& mStringMember == mStringMember
				&& mPairMember == mPairMember
				&& mTupleMember == mTupleMember
				&& mForwardListMember == mForwardListMember
				&& mListMember == mListMember
				&& mMapMember == mMapMember
				&& mSetMember == mSetMember
				&& mUnorderedMapMember == mUnorderedMapMember
				&& mUnorderedSetMember == mUnorderedSetMember
				&& mArrayMember == mArrayMember;
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