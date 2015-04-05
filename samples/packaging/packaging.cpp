#include <iostream>
#include <sstream>
#include <xnet/serialization/deserializer.hpp>
#include <xnet/serialization/formats/text_source.hpp>
#include <xnet/serialization/serializer.hpp>
#include <xnet/serialization/formats/text_sink.hpp>
#include <xnet/serialization/split.hpp>
#include <xnet/serialization/formats/generic_sink.hpp>
#include <xnet/serialization/formats/generic_source.hpp>
#include <xnet/serialization/formats/xml_sink.hpp>
#include <xnet/serialization/formats/xml_source.hpp>
#include <xnet/serialization/serialization_error.hpp>
#include <xnet/detail/rapidxml/rapidxml_print.hpp>
#include <xnet/serialization/types/enum.hpp>
#include <xnet/serialization/types/pair.hpp>
#include <xnet/serialization/types/tuple.hpp>
#include <xnet/serialization/types/vector.hpp>
#include <xnet/serialization/types/list.hpp>
#include <xnet/serialization/types/forward_list.hpp>
#include <xnet/serialization/types/unordered_set.hpp>
#include <xnet/serialization/types/unordered_map.hpp>
#include <xnet/serialization/types/map.hpp>
#include <xnet/serialization/types/array.hpp>


enum class TestEnum: unsigned char
{
	A = 0,
	B
};

XNET_SERIALIZATION_ENUM(TestEnum);

struct Point
{
	int x;
	int y;
	TestEnum e;
	std::pair<int, int> p;
	std::tuple<char, int, std::string> t;
	std::forward_list<std::string> v;
	std::unordered_set<std::string> mySet;
	std::unordered_map<int, std::string> myMap;
	std::array<int, 5> arr;

	template<typename S>
	void serialize(S& s)
	{
		XNET_CURRENT_TYPE(s, *this, Point);
		s & XNET_TAGVAL(x) & XNET_TAGVAL(y) & XNET_TAGVAL(e) & XNET_TAGVAL(p) & XNET_TAGVAL(t) & XNET_TAGVAL(v) & XNET_TAGVAL(mySet) & XNET_TAGVAL(myMap) & XNET_TAGVAL(arr);
	}
};
/*
template<typename S>
void serialize(S& s, Point& p)
{
	//s & p.x & p.y;
	xnet::serialization::split(s, p);
}

template<typename S>
void save(S& s, const Point& p)
{
	s << p.x << p.y;
}

template<typename S>
void load(S& s, Point& p)
{
	s >> p.x >> p.y;
}*/


int main()
{

	std::string buf;
	rapidxml::xml_document<> doc;
	auto root = doc.allocate_node(rapidxml::node_element, "package");
	doc.append_node(root);

	{
		typedef xnet::serialization::xml_sink xmls;
		xmls sink(&doc, root);
		xnet::serialization::serializer<xmls> s(sink);

		Point p{ 13, 19,
			TestEnum::B,
			std::make_pair(99, 100),
			std::make_tuple('a', 12, "test"),
			{ "test", "abv", "oba" },
			{ "b", "x", "a" },
			{ std::make_pair(4, "aber"), std::make_pair(1, "doch") },
			{1,2,3,4,5}
		};
		s << XNET_TAGVAL(p);
	}
	rapidxml::print(std::cout, doc);

	{
		typedef xnet::serialization::xml_source xmls;
		xmls source(root->first_node());
		xnet::serialization::deserializer<xmls> s(source);

		Point p {0, 0 };
		try {
			s >> XNET_TAGVAL(p);
			std::cout << p.x << ", " << p.y << std::endl;
		}
		catch (xnet::serialization_error e)
		{
			std::cout << e.what() << std::endl;
		}
	}

	/*{
		std::ostringstream oss(buf);

		typedef xnet::serialization::text_sink<char> ts;
		ts texts(oss);
		xnet::serialization::serializer<ts> s(texts);

		Point p{ 13, 19 };
		s << p;
		buf = oss.str();
	}


	/*{
		std::istringstream iss(buf);

		typedef xnet::serialization::text_source<char> ts;
		ts texts(iss);
		xnet::serialization::deserializer<ts> des(texts);

		Point p;
		des >> p;
	}*/


#ifdef WIN32
	std::cin.get();
#endif

	return 0;
}