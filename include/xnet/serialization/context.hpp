#pragma once
#ifndef _XNET_SERIALIZATION_CONTEXT_HPP
#define _XNET_SERIALIZATION_CONTEXT_HPP

#include <tuple>
#include "../detail/variadic_utils.hpp"

namespace xnet {
	namespace serialization {
		namespace detail {
			template<typename C, typename Tuple, std::size_t I>
			struct get_tuple_item_by_type
			{
				static_assert(I < std::tuple_size<Tuple>::value, "Can not find item of type C!");
			private:
				typedef typename std::decay<C>::type decay_t_type;
				typedef typename std::tuple_element<I, Tuple>::type elem_type;
				typedef typename std::decay<elem_type>::type decay_elem_type;

				struct get_func
				{
					static C& get(Tuple& t)
					{
						return std::get<I>(t);
					}
				};

				template<bool = std::is_same<decay_elem_type, decay_t_type>::value>
				struct inner_get
				{
					typedef typename get_tuple_item_by_type<C, Tuple, I + 1>::func type;
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

		template<typename... Context>
		class context
		{
			template<typename... Other>
			friend class context;
		public:
			typedef std::tuple<Context&...> inner_type;
		public:
			context(Context&... content)
				: _content(content...)
			{
			}

			template<typename... AddC>
			context<Context..., AddC...> with(AddC&... cx)
			{
				return context<Context..., AddC...>(std::tuple_cat(_content, std::forward_as_tuple(cx...)));
			}

			template<typename C>
			C& get()
			{
				return detail::get_tuple_item_by_type<C, inner_type, 0>::func::get(_content);
			}
			
		private:
			context(inner_type&& content)
				: _content(content)
			{
			}
		private:
			inner_type _content;
		};

		template<typename... Context>
		context<Context...> make_context(Context&... cx)
		{
			return context<Context...>(cx...);
		}

		template<typename Context>
		struct is_context
			: public ::xnet::detail::is_specialization<Context, context>
		{
		};
	}
}


#endif