#pragma once
#ifndef _XNET_DETAIL_MULTICAST_PROTOCOL_HPP
#define _XNET_DETAIL_MULTICAST_PROTOCOL_HPP

#include <string>
#include <random>
#include <boost/asio.hpp>
#include <boost/lexical_cast.hpp>

namespace xnet {
	namespace detail {

		template<typename ContentType>
		class multicast_protocol
		{
		public:
			typedef ContentType content_type;

			enum class Command : char
			{
				Scan = 's',		// Scan for other peers without sending own informations
				Announce = 'a'	// inform peer about us (expect no answer)
			};

			static boost::asio::ip::address generate_address(bool ipv4, const std::string& token)
			{
				boost::asio::ip::address address;
				if (ipv4)
					address = generate_v4_address(token);
				else
					address = generate_v6_address(token);

				BOOST_ASSERT(address.is_multicast());
				return address;
			}

			static boost::asio::ip::address_v4 generate_v4_address(const std::string& token)
			{
				auto engine = _build_random_engine(token);

				typedef boost::asio::ip::address_v4::bytes_type byte_types;
				std::uniform_int_distribution<unsigned short> ipDist(0, std::numeric_limits<byte_types::value_type>::max());

				return boost::asio::ip::address_v4
				{
					byte_types
					{
						239,			// private use multicast range
						static_cast<byte_types::value_type>(ipDist(engine)),
						static_cast<byte_types::value_type>(ipDist(engine)),
						static_cast<byte_types::value_type>(ipDist(engine))
					}
				};
			}

			static boost::asio::ip::address_v6 generate_v6_address(const std::string& token)
			{
				auto engine = _build_random_engine(token);

				typedef boost::asio::ip::address_v6::bytes_type byte_types;
				std::uniform_int_distribution<unsigned short> ipDist(0, std::numeric_limits<byte_types::value_type>::max());

				return boost::asio::ip::address_v6
				{
					byte_types
					{
						0xFF,	// use multicast
						0x08,	// global scope
						static_cast<byte_types::value_type>(ipDist(engine)),
						static_cast<byte_types::value_type>(ipDist(engine)),
						static_cast<byte_types::value_type>(ipDist(engine)),
						static_cast<byte_types::value_type>(ipDist(engine)),
						static_cast<byte_types::value_type>(ipDist(engine)),
						static_cast<byte_types::value_type>(ipDist(engine)),
						static_cast<byte_types::value_type>(ipDist(engine)),
						static_cast<byte_types::value_type>(ipDist(engine)),
						static_cast<byte_types::value_type>(ipDist(engine)),
						static_cast<byte_types::value_type>(ipDist(engine)),
						static_cast<byte_types::value_type>(ipDist(engine)),
						static_cast<byte_types::value_type>(ipDist(engine)),
						static_cast<byte_types::value_type>(ipDist(engine)),
						static_cast<byte_types::value_type>(ipDist(engine))
					}
				};
			}

			static unsigned short generate_port(const std::string& token)
			{
				auto engine = _build_random_engine(token);
				std::uniform_int_distribution<unsigned short> portDist(1025);
				return portDist(engine);
			}

			template<typename Iter>
			static bool parse_announce_message(Iter begin, Iter end, const std::string& expToken, uint64_t& id, content_type& content)
			{
				boost::match_results<Iter> matches;
				if (!_parse_message(begin, end, expToken, matches))
				{
					return false;
				}

				if (*matches["cmd"].first != static_cast<char>(Command::Announce))
					return false;

				id = boost::lexical_cast<uint64_t>(matches["id"]);

				auto& contentMatch = matches["content"];
				return content.deserialize(contentMatch.first, contentMatch.second);
			}

			template<typename Iter>
			static bool parse_scan_message(Iter begin, Iter end, const std::string& expToken)
			{
				boost::match_results<Iter> matches;
				return _parse_message(begin, end, expToken, matches)
					&& *matches["cmd"].first == static_cast<char>(Command::Scan);
			}

			static std::string build_scan_message(const std::string& token)
			{
				return token + ";0;" + static_cast<char>(Command::Scan) + ";";
			}

			static std::string build_announce_message(const std::string& token, const std::string& id, const content_type& content)
			{
				return token + ';' + id + ";" + static_cast<char>(Command::Announce) + ";" + content.serialize();
			}

		private:
			template<typename Iter>
			static bool _parse_message(Iter begin, Iter end, const std::string& expToken, boost::match_results<Iter>& matches)
			{
				static boost::regex messageHeaderFormat("(?<token>[[:alpha:]]+);(?<id>\\d{1,20});(?<cmd>s|a);(?<content>.*)");
				if (!boost::regex_match(begin, end, matches, messageHeaderFormat))
					return false;

				const std::string token = matches["token"];

				return token == expToken;
			}

			static std::default_random_engine _build_random_engine(const std::string& token)
			{
				typedef std::default_random_engine::result_type seed_type;
				seed_type seed = 0;
				std::uniform_int_distribution<seed_type> seedDist(0, std::numeric_limits<seed_type>::max());
				std::default_random_engine engine(seed);

				for (auto c : token)
				{
					engine.seed(seed);
					seed = seedDist(engine);
				}

				return engine;
			}
		};

	}
}

#endif