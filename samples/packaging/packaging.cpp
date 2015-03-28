#include <iostream>
#include <sstream>
#include <xnet/serialization/deserializer.hpp>
#include <xnet/serialization/formats/text_source.hpp>
#include <xnet/serialization/serializer.hpp>
#include <xnet/serialization/formats/text_sink.hpp>
#include <xnet/serialization/split.hpp>

struct Point
{
	int x;
	int y;

	template<typename S>
	void serialize(S& s)
	{
		xnet::serialization::split_this(this, s);
	}

	template<typename S>
	void save(S& s) const
	{
		s << x << y;
	}

	template<typename S>
	void load(S& s)
	{
		s >> x >> y;
	}
};

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
}


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


#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

class gps_position
{
public:
	int degrees;
	int minutes;
	float seconds;
	gps_position(){};
	gps_position(int d, int m, float s) :
		degrees(d), minutes(m), seconds(s)
	{}
};

namespace boost {
namespace serialization {

template<class Archive>
void serialize(Archive & ar, gps_position & g, const unsigned int version)
{
	ar & g.degrees;
	ar & g.minutes;
	ar & g.seconds;
}

} // namespace serialization
} // namespace boost


int main2() {
	// create class instance
	const gps_position g(35, 59, 24.567f);

	// save data to archive
	{
		boost::archive::text_oarchive oa(std::cout);
		// write class instance to archive
		oa << g;
		// archive and stream closed when destructors are called
	}
	return 0;
}