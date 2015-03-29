#include <iostream>
#include <sstream>
#include <xnet/serialization/deserializer.hpp>
#include <xnet/serialization/formats/text_source.hpp>
#include <xnet/serialization/serializer.hpp>
#include <xnet/serialization/formats/text_sink.hpp>
#include <xnet/serialization/split.hpp>
#include <xnet/serialization/formats/generic_sink.hpp>
#include <xnet/serialization/formats/generic_source.hpp>

struct Point
{
	int x;
	int y;

	template<typename S>
	void serialize(S& s)
	{
		s & xnet::tagval("x", x) & xnet::tagval("y", y);
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
	{
		std::ostringstream oss(buf);

		typedef xnet::serialization::text_sink<char> ts;
		ts texts(oss);
		xnet::serialization::serializer<ts> s(texts);

		Point p{ 13, 19 };
		s << p;
		buf = oss.str();
	}

	std::cout << buf << std::endl;

	{
		std::istringstream iss(buf);

		typedef xnet::serialization::text_source<char> ts;
		ts texts(iss);
		xnet::serialization::deserializer<ts> des(texts);

		Point p;
		des >> p;
	}


#ifdef WIN32
	std::cin.get();
#endif

	return 0;
}