#pragma once
#ifndef _XNET_LAN_DISCOVERER_HPP
#define _XNET_LAN_DISCOVERER_HPP


#include <boost/asio.hpp>
#include <boost/optional.hpp>
#include <boost/regex.hpp>
#include <boost/assert.hpp>
#include <random>
#include <chrono>
#include "detail/return_on_error.hpp"
#include "lan_discovery_content.hpp"

namespace xnet {

	template<typename ContentType>
	class basic_lan_discoverer
	{
	public:
		typedef ContentType				content_type;
		typedef boost::asio::ip::udp	protocol_type;
		typedef protocol_type::socket	socket_type;
		typedef protocol_type::endpoint endpoint_type;

		class result
		{
			template<typename Tmpl>
			friend class basic_lan_discoverer;
		public:
			result()
			{
			}

			result(const endpoint_type& from)
				: _recvfrom(from)
			{
			}

			const content_type& content() const { return _content; }
			const endpoint_type& endpoint() const { return _recvfrom; }
			const std::string& identifier() const { return _identifier; }

		private:
			content_type _content;
			endpoint_type _recvfrom;
			std::string _identifier;
		};

	private:
		enum class Command : char
		{
			Scan = 'd',		// Scan for other peers without sending own informations
			Announce = 'a'	// inform peer about us (expect no answer)
		};
		typedef std::function<void(const boost::system::error_code&, const result&)> _discovered_handler;
		typedef std::array<char, 2048> recvbuffer_type;

	public:
		basic_lan_discoverer(boost::asio::io_service& service, const std::string& token)
			: _service(service)
			, _sendStrand(service)
			, _socket(service)
			, _token(token)
			, _identifier(_create_random_identifier())
			, _messageHeaderFormat("(?<token>[[:alpha:]]+);(?<id>\\d{1,20});(?<cmd>d|a);(?<content>.*)")
		{
		}

		~basic_lan_discoverer()
		{
		}

		void open(const protocol_type& protocol, unsigned short port)
		{
			boost::system::error_code ec;
			open(protocol, port, ec);
			boost::asio::detail::throw_error(ec);
		}

		boost::system::error_code open(const protocol_type& protocol, unsigned short port, boost::system::error_code& ec)
		{
			_socket.open(protocol, ec);
			XNET_RETURN_ERROR(ec);

			_socket.bind(endpoint_type(protocol, port), ec);
			if (ec)
			{
				_socket.close();
				return ec;
			}
			_socket.set_option(boost::asio::socket_base::broadcast(true));
			return ec;
		}

		bool is_open() const
		{
			return _socket.is_open();
		}

		void announce(unsigned short port, const content_type& content)
		{
			boost::system::error_code ec;
			announce(port, content, ec);
			boost::asio::detail::throw_error(ec);
		}

		boost::system::error_code announce(unsigned short port, const content_type& content, boost::system::error_code& ec)
		{
			auto message = _build_message(Command::Announce, content);
			_socket.send_to(boost::asio::buffer(message), protocol_type::endpoint(boost::asio::ip::address_v4::broadcast(), port), 0, ec);
			return ec;
		}

		void scan(unsigned short port)
		{
			boost::system::error_code ec;
			scan(port, ec);
			boost::asio::detail::throw_error(ec);
		}

		boost::system::error_code scan(unsigned short port, boost::system::error_code& ec)
		{
			auto message = _build_message(Command::Scan);
			_socket.send_to(boost::asio::buffer(message), protocol_type::endpoint(boost::asio::ip::address_v4::broadcast(), port), 0, ec);
			return ec;
		}
		
		result discover()
		{
			boost::system::error_code ec;
			auto res = discover(ec);
			boost::asio::detail::throw_error(ec);
			return res;
		}

		result discover(boost::system::error_code& ec)
		{
			do {
				auto len = _socket.receive_from(boost::asio::buffer(_recvBuffer), _recvEndpoint, 0, ec);
				XNET_RETURN_ON_ERROR(ec, result());

				Command c;
				result res(_recvEndpoint);
				if (_parse_message(len, res._identifier, c, res._content)
					&& c == Command::Announce)
				{
					return res;
				}

			} while (true);
		}

		template<typename DiscoverHandler>
		BOOST_ASIO_INITFN_RESULT_TYPE(DiscoverHandler,
			void(boost::system::error_code, result))
			async_discover(BOOST_ASIO_MOVE_ARG(DiscoverHandler) handler)
		{
			boost::asio::detail::async_result_init<
				DiscoverHandler, void(boost::system::error_code, result)> init(
				BOOST_ASIO_MOVE_CAST(DiscoverHandler)(handler));

			//_async_search(init.handler);

			return init.result.get();
		}

		void close()
		{
			_socket.close();
		}

		boost::system::error_code close(boost::system::error_code& ec)
		{
			return _socket.close(ec);
		}
	private:
		bool _parse_message(std::size_t bufLen, std::string& id, Command& cmd, content_type& content)
		{
			auto begin = _recvBuffer.cbegin();
			auto end = begin + bufLen;
			boost::match_results<recvbuffer_type::const_iterator> matches;
			if (!boost::regex_match(begin, end, matches, _messageHeaderFormat))
				return false;

			const std::string token = matches["token"];
			id = matches["id"];

			if (token != _token)
				return false;

			switch (*matches["cmd"].first)
			{
			case static_cast<char>(Command::Scan) :
				cmd = Command::Scan;
				break;
			case static_cast<char>(Command::Announce):
				cmd = Command::Announce;
				break;
			default:
				BOOST_ASSERT(false);
			}

			auto& contentMatch = matches["content"];
			return content.deserialize(contentMatch.first, contentMatch.second);
		}

		std::string _build_message(Command cmd)
		{
			return _token + ';' + _identifier + ";" + static_cast<char>(cmd) + ";";
		}

		std::string _build_message(Command cmd, const content_type& content)
		{
			return _token + ';' + _identifier + ";" + static_cast<char>(cmd) + ";" + content.serialize();
		}

		static std::string _create_random_identifier()
		{
			typedef uint64_t idnum_type;
			auto maxIdNum = std::numeric_limits<idnum_type>::max();
			std::uniform_int_distribution<uint64_t> id_distribution(1, maxIdNum);
			auto seed = static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count());
			std::default_random_engine engine(seed);
			auto numId = id_distribution(engine);

			auto idString = std::to_string(numId);
			BOOST_ASSERT(idString.size() <= 20);
			return idString;
		}
	private:
		const std::string _identifier;
		const std::string _token;
		boost::asio::io_service& _service;
		boost::asio::io_service::strand _sendStrand;
		socket_type _socket;

		recvbuffer_type _recvBuffer;
		endpoint_type _recvEndpoint;

		const boost::regex _messageHeaderFormat;
	};

	typedef basic_lan_discoverer<lan_discovery_content> lan_discoverer;
}




#endif