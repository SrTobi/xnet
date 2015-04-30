#pragma once
#ifndef _XNET_SERIALIZATION_TYPE_VARIAN_HPP
#define _XNET_SERIALIZATION_TYPE_VARIAN_HPP
	
#include <boost/mpl/front.hpp>
#include <boost/mpl/pop_front.hpp>
#include <boost/mpl/eval_if.hpp>
#include <boost/mpl/identity.hpp>
#include <boost/mpl/size.hpp>
#include <boost/mpl/empty.hpp>
#include <boost/variant.hpp>
#include "../serialization_error.hpp"

namespace xnet {
	namespace serialization {
		namespace detail {
			template<class S>
			struct variant_save_visitor: boost::static_visitor<> 
			{
				variant_save_visitor(S& s) :
					_serializer(s)
				{}
				template<class T>
				void operator()(const T& value) const
				{
					_serializer << value;
				}
			private:
				S& _serializer;
			};

			template<typename Sink, typename Context, BOOST_VARIANT_ENUM_PARAMS(class T)>
			void split_serialize_variant(serializer<Sink, Context>& s, const boost::variant<BOOST_VARIANT_ENUM_PARAMS(T)>& v)
			{
				s << v.which();
				variant_save_visitor<serializer<Sink, Context>> sv(s);
				v.apply_visitor(sv);
			}

			template<class Types>
			struct variant_impl {
				typedef typename int which_type;

				struct load_null {
					template<class S, class V>
					static void invoke(S&, which_type, V &){}
				};

				struct load_impl {
					template<class S, class V>
					static void invoke(S& s, which_type which, V & v)
					{
						if(which == 0){
							// note: A non-intrusive implementation (such as this one)
							// necessary has to copy the value.  This wouldn't be necessary
							// with an implementation that de-serialized to the address of the
							// aligned storage included in the variant.
							typedef BOOST_DEDUCED_TYPENAME boost::mpl::front<Types>::type head_type;
							head_type value;
							s >> value;
							v = value;
							//s.reset_object_address(& boost::get<head_type>(v), & value);
							return;
						}
						typedef BOOST_DEDUCED_TYPENAME boost::mpl::pop_front<Types>::type type;
						variant_impl<type>::load(s, which - 1, v);
					}
				};

				template<class S, class V>
				static void load(S& s, which_type which, V& v){
					typedef BOOST_DEDUCED_TYPENAME boost::mpl::eval_if<boost::mpl::empty<Types>,
						boost::mpl::identity<load_null>,
						boost::mpl::identity<load_impl>
					>::type typex;
					typex::invoke(s, which, v);
				}
			};

			template<typename Source, typename Context, BOOST_VARIANT_ENUM_PARAMS(class T)>
			void split_serialize_variant(deserializer<Source, Context>& s, boost::variant<BOOST_VARIANT_ENUM_PARAMS(T)>& v)
			{
				typedef boost::variant<BOOST_VARIANT_ENUM_PARAMS(T)> variant_type;
				typedef typename int which_type;
				typedef typename variant_type::types types;

				which_type which;
				s >> which;

				if (which >= boost::mpl::size<types>::value)
				{
					throw serialization_error("Variant type does not exist!");
				}
				
				variant_impl<types>::load(s, which, v);
			}
		}

		template<typename S, BOOST_VARIANT_ENUM_PARAMS(class T)>
		void serialize(S& s, boost::variant<BOOST_VARIANT_ENUM_PARAMS(T)>& v)
		{
			s.current_type("boost::variant");
			detail::split_serialize_variant(s, v);
		}
	}
}


#endif