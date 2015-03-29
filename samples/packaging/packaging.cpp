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

#include <xnet/detail/rapidxml/rapidxml_print.hpp>

struct Point
{
	int x;
	int y;

	template<typename S>
	void serialize(S& s)
	{
		XNET_CURRENT_TYPE(s, *this, Point);
		s & XNET_TAGVAL(x) & XNET_TAGVAL(y);
	}
};

/*template<typename S>
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

		Point p{ 13, 19 };
		s << XNET_TAGVAL(p);
	}
	rapidxml::print(std::cout, doc);

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