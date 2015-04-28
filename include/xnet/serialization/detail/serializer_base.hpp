#pragma once
#ifndef _XNET_SERIALIZATION_DETAIL_SERIALIZER_BASE_HPP
#define _XNET_SERIALIZATION_DETAIL_SERIALIZER_BASE_HPP

#include <tuple>

namespace xnet {
	namespace serialization {
		namespace detail {
			template<typename Inner>
			class serializer_base
			{
			public:
				Inner& inner() { return static_cast<Inner&>(*this); }
			};

			template<typename>
			struct make_ref_tuple;

			template<typename... Items>
			struct make_ref_tuple< std::tuple<Items...> >
			{
				typedef std::tuple<Items&...> type;
			};

			template<typename T, typename Tuple, std::size_t I>
			struct get_tuple_item_by_type
			{
				static_assert(I < std::tuple_size<Tuple>::value, "Can not find item of type T!");
			private:
				typedef typename std::decay<T>::type decay_t_type;
				typedef typename std::tuple_element<I, Tuple>::type elem_type;
				typedef typename std::decay<elem_type>::type decay_elem_type;

				struct get_func
				{
					static T& get(Tuple& t)
					{
						return std::get<I>(t);
					}
				};

				template<bool = std::is_same<decay_elem_type, decay_t_type>::value>
				struct inner_get
				{
					typedef typename get_tuple_item_by_type<T, Tuple, I + 1>::func type;
				};

				template<>
				struct inner_get<true>
				{
					typedef get_func type;
				};

			public:
				typedef typename inner_get<>::type func;
			};
		}
	}
}

#endif