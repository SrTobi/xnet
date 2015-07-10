#pragma once
#ifndef _XNET_PACKAGE_SOCKET_ADAPTER_HPP
#define _XNET_PACKAGE_SOCKET_ADAPTER_HPP

#include <boost/asio/buffer.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <vector>
#include "package_factory.hpp"


namespace xnet {

	template<typename Socket>
	class package_socket_adapter
	{
	private:
		struct message_header
		{
		public:
			enum max_size_t : uint32_t { max_size = ~0x80000000 /* = 0111 1111  1111 1111  1111 1111  1111 1111*/ };
			message_header()
			{
				_buffer[0] = 0;
				_buffer[7] = 0xFF;
			}

			message_header(const package& p)
			{
				BOOST_ASSERT(p.size() <= max_size);
				write_num_at<uint32_t, 0>(p.size());
				_buffer[0] |= 0x80 /* 1000 0000 */;
				auto name = p.format().name();
				std::copy(name, name + 4, _buffer.begin() + 4);
			}

			uint32_t message_size() const
			{
				BOOST_ASSERT(_buffer[0]);
				if (is_advanced())
				{
					return read_num_at<uint32_t, 0>()
						& max_size;
				}
				else if (is_extended())
				{
					return read_num_at<uint16_t, 0>()
						& ~0xC000 /* = 0011 1111  1111 1111 */;
				}
				else{
					return read_num_at<uint8_t, 0>();
				}
			}

			package_format message_format() const
			{
				BOOST_ASSERT(_buffer[0]);
				if (is_advanced())
				{
					return { _buffer[4], _buffer[5], _buffer[6], '\0' };
				}
				else{
					throw std::logic_error("not implemented!");
				}

			}

			uint32_t header_size() const
			{
				if (is_advanced())
				{
					return 8;
				}
				else if (is_extended())
				{
					return 2;
				}
				else{
					return 1;
				}
			}

			boost::asio::const_buffers_1 send_buffer() const
			{
				return boost::asio::buffer(_buffer);
			}

			boost::asio::mutable_buffers_1 recv_header_buffer()
			{
				return asio::buffer(_buffer.data(), 1);
			}

			boost::asio::mutable_buffers_1 recv_header_ext_buffer()
			{
				BOOST_ASSERT(is_advanced() || is_extended());
				const auto s = header_size() - 1;
				BOOST_ASSERT(s != 0);
				return asio::buffer(_buffer.data() + 1, s);
			}

		private:
			bool is_advanced() const
			{
				return (_buffer[0] & 0x80) /* 1000 0000 */ != 0;
			}

			bool is_extended() const
			{
				BOOST_ASSERT(!is_advanced());
				return (_buffer[0] & 0x40) /* 0100 0000 */ != 0;
			}

			template<typename T, unsigned int Pos>
			T read_num_at() const
			{
				static_assert(sizeof(_buffer) == 8, "buffer has a wrong size!");
				static_assert(std::is_unsigned<T>::value, "T must be unsigned!");
				T result = T(0);
				const auto start = Pos;
				const auto end = Pos + sizeof(T);

				unsigned int bsh = 0;
				auto i = end;
				do
				{
					--i;
					result |= _buffer[i] << bsh;
					bsh += 8;
				} while (i > start);

				return result;
			}

			template<typename T, unsigned int Pos>
			void write_num_at(T _val)
			{
				auto end = Pos + sizeof(_val);
				auto bsh = sizeof(_val) * 8;
				for (auto i = Pos; i < end; ++i)
				{
					bsh -= 8;
					_buffer[i] = (_val >> bsh) & 0xFF;
				}
			}

			/*
				Simple:
					1233 3333  [4444 4444]

					1: Simple or advanced bit [0 = simple, 1 = advanced]
					2: Extended size [0 = only 3 for size, 1 = 3 & 4 for the size]
					3: Size of message
					4: additional size of message bits (only if [2] = 1)
				Advanced:
					1555 5555  5555 5555  5555 5555  5555 5555  6666 6666  6666 6666  6666 6666  6666 6666
					5: Size bits
					6: Format bits
			*/
			std::array<unsigned char, sizeof(uint32_t) + sizeof(package_format::id_type)> _buffer;
		};
	public:
		typedef typename Socket::lowest_layer_type lowest_layer_type;
		typedef Socket next_layer_type;
	public:
		template<typename... Args>
		package_socket_adapter(package_factory& pFactory, Args&&... args)
			: _socket(std::forward<Args>(args)...)
			, _pFactory(pFactory)
		{
		}

		void send(const package& pack)
		{
			boost::system::error_code ec;
			send(pack, ec);
			boost::asio::detail::throw_error(ec);
		}

		void send(const package& pack, boost::system::error_code& ec)
		{
			message_header header(pack);
			boost::asio::write(_socket, header.send_buffer());
			boost::asio::write(_socket, pack.underlying_data(), ec);
		}

		template<typename SendHandler>
		BOOST_ASIO_INITFN_RESULT_TYPE(SendHandler,
			void(boost::system::error_code, std::size_t))
			async_send(const package& pack, BOOST_ASIO_MOVE_ARG(SendHandler) handler)
		{
			boost::asio::detail::async_result_init<
				SendHandler, void(boost::system::error_code, std::size_t)> init(
				BOOST_ASIO_MOVE_CAST(SendHandler)(handler));

			struct data_bundle
			{
				data_bundle(const package& p)
					: _header(p)
					, _package(p)
				{
					const auto& ud = p.underlying_data();
					_sendBuffer.reserve(ud.size() + 1);
					_sendBuffer.push_back(_header.send_buffer());
					_sendBuffer.insert(_sendBuffer.end(), ud.begin(), ud.end());
				}

				message_header _header;
				package _package;
				std::vector<boost::asio::const_buffer> _sendBuffer;
			};
			
			auto b = std::make_shared<data_bundle>(pack);
			auto innerHandler = std::move(init.handler);
			boost::asio::async_write(_socket, b->_sendBuffer, 
				[this, b, innerHandler](const boost::system::error_code& ec, std::size_t len)
				{
					innerHandler(ec, len);
				});

			return init.result.get();
		}

		xnet::package receive()
		{
			boost::system::error_code ec;
			auto p = receive(pack, ec);
			boost::asio::detail::throw_error(ec);
			return p;
		}

		xnet::package receive(boost::system::error_code& ec)
		{
			return nullptr;
		}

		template<typename RecvHandler>
		BOOST_ASIO_INITFN_RESULT_TYPE(RecvHandler,
			void(boost::system::error_code, xnet::package))
			async_receive(BOOST_ASIO_MOVE_ARG(RecvHandler) handler)
		{
			boost::asio::detail::async_result_init<
				RecvHandler, void(boost::system::error_code, xnet::package)> init(
				BOOST_ASIO_MOVE_CAST(RecvHandler)(handler));

			auto innerHandler = std::move(init.handler);
			_recv_header = message_header();
			boost::asio::async_read(_socket, _recv_header.recv_header_buffer(),
				[this, innerHandler](const boost::system::error_code& ec, std::size_t len) 
				{
					auto hndl = [this, innerHandler]()
					{
						_recv_buffer.reserve(_recv_header.message_size() + 1);
						_recv_buffer.resize(_recv_header.message_size());
						boost::asio::async_read(_socket, boost::asio::buffer(_recv_buffer),
							[this, innerHandler](const boost::system::error_code& ec, std::size_t len) 
							{
								if (!ec)
								{
									_recv_buffer.push_back(0);
									auto p = _pFactory.read_package(_recv_buffer);
									innerHandler(ec, p);
								}
								else{
									innerHandler(ec, nullptr);
								}
							});
					};

					if (!ec)
					{
						if (_recv_header.header_size() > 1)
						{
							boost::asio::async_read(_socket, _recv_header.recv_header_ext_buffer(),
								[this, innerHandler, hndl](const boost::system::error_code& ec, std::size_t len) 
								{
									if (!ec)
									{
										hndl();
									}
									else{
										innerHandler(ec, nullptr);
									}
								});
						}
						else{
							hndl();
						}
					}
					else{
						innerHandler(ec, nullptr);
					}
				});

			return init.result.get();
		}


		const next_layer_type& next_layer() const
		{
			return _socket;
		}

		next_layer_type& next_layer()
		{
			return _socket;
		}

		const lowest_layer_type& lowest_layer() const
		{
			return _socket.lowest_layer();
		}

		lowest_layer_type& lowest_layer()
		{
			return _socket.lowest_layer();
		}

	private:
		package_factory& _pFactory;
		Socket _socket;
		message_header _recv_header;
		std::vector<unsigned char> _recv_buffer;
	};

}


#endif