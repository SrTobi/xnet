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
#include <xnet/serialization/types/enum.hpp>
#include <xnet/serialization/types/variant.hpp>

namespace serialization_test {
	enum class TestEnum
	{
		A = 1,
		B = 2
	};
	XNET_SERIALIZATION_ENUM(TestEnum)

	template<int I>
	struct NumberContext
	{
		int number = 999;
		bool ok() { return number == I; }
	};

	struct EmptyStruct
	{
		template<typename S>
		void serialize(S& s)
		{
		}
	};

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
		TestEnum mEnumMember = TestEnum::B;
		boost::variant<int, std::string> mVariantMember = 4;
		EmptyStruct mEmptyStructMember;


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
			, mEnumMember(TestEnum::A)
			, mVariantMember("test")
		{
		}
		
		template<typename S>
		void serialize(S& s)
		{
			XNET_CURRENT_TYPE(s, *this, SerializationTestClass);
			s.context<NumberContext<1>>().number = 1;
			s.context<NumberContext<2>>().number = 2;
			s.context<NumberContext<3>>().number = 3;
			s & mBoolMember & mIntMember & mStringMember & mPairMember & mTupleMember & mForwardListMember
				& mListMember & mMapMember & mSetMember & mUnorderedSetMember & mUnorderedMapMember & mArrayMember
				& mEnumMember & mVariantMember & mEmptyStructMember;
		}

		bool operator ==(const SerializationTestClass& right) const
		{
			return mBoolMember == right.mBoolMember
				&& mIntMember == right.mIntMember
				&& mStringMember == right.mStringMember
				&& mPairMember == right.mPairMember
				&& mTupleMember == right.mTupleMember
				&& mForwardListMember == right.mForwardListMember
				&& mListMember == right.mListMember
				&& mMapMember == right.mMapMember
				&& mSetMember == right.mSetMember
				&& mUnorderedMapMember == right.mUnorderedMapMember
				&& mUnorderedSetMember == right.mUnorderedSetMember
				&& mArrayMember == right.mArrayMember
				&& mEnumMember == right.mEnumMember
				&& mVariantMember == right.mVariantMember;
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
		typedef xnet::serialization::context<NumberContext<1>, NumberContext<2>> Contx;
		NumberContext<1> n1;
		NumberContext<2> n2;
		NumberContext<3> n3;
		xnet::serialization::serializer<Sink, Contx> s(sink, xnet::serialization::make_context(n1, n2));
		auto newS = s.with_context(n3);
		newS << reference_instance;
		BOOST_CHECK(n1.ok());
		BOOST_CHECK(n2.ok());
		BOOST_CHECK(n3.ok());
	}

	template<typename Source>
	void test_source_format(Source& source)
	{
		typedef xnet::serialization::context<NumberContext<1>> Contx;
		NumberContext<1> n1;
		NumberContext<2> n2;
		NumberContext<3> n3;
		xnet::serialization::deserializer<Source, Contx> s(source, xnet::serialization::make_context(n1));
		auto newS = s.with_context(n2, n3);
		SerializationTestClass c;
		newS >> c;
		BOOST_CHECK_EQUAL(c, reference_instance);
		BOOST_CHECK(n1.ok());
		BOOST_CHECK(n2.ok());
		BOOST_CHECK(n3.ok());
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