#include <iostream>
#include <sstream>
#include "xnet/serialization/serialization.hpp"
#include "xnet/package_factory.hpp"


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


int main()
{
	xnet::package_factory fac;

	Point p{ 1, 2 };

	xnet::package pack = fac.make_package(p);
	xnet::package pack2 = fac.make_package(p, pack);

	{
		xnet::package next;
		Point p2 = xnet::unpack_package<Point>(pack2, next);
		std::cout << p2.x << p2.y << std::endl;
	}

	/*
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