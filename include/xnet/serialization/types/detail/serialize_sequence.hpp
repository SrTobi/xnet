#pragma once
#ifndef XNET_DETAIL_SERIALIZATION_SERIALIZE_SEQUENCE_HPP
#define XNET_DETAIL_SERIALIZATION_SERIALIZE_SEQUENCE_HPP

namespace xnet {
	namespace serialization {
		namespace detail {
			template<typename S, typename Iter>
			void save_sequence(S& s, Iter begin, Iter end, const char* tag)
			{
				s.begin_sequence_save(tag);

				while(begin != end)
				{
					s << *begin;
					++begin;
				}

				s.end_sequence_save(tag);
			}

			template<typename S, typename Iter>
			void save_sequence(S& s, Iter begin, Iter end, std::size_t size, const char* tag)
			{
				s.begin_sequence_save(tag, size);

				while(begin != end)
				{
					s << *begin;
					++begin;
				}

				s.end_sequence_save(tag);
			}

			template<typename S, typename Handler>
			void load_unsized_sequence(S& s, Handler& handler)
			{
				
			}
		}
	}
}


#endif