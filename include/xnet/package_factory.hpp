#pragma once
#ifndef _XNET_PACKAGE_FACTORY_HPP
#define _XNET_PACKAGE_FACTORY_HPP

#include <memory>
#include <functional>
#include <vector>
#include "package.hpp"

namespace xnet {
	namespace serialization {
		class generic_sink;
	}

	class package_content_factory
	{
	public:
		typedef std::function<void(serialization::generic_sink&)> constr_type;
	public:
		inline package_content_factory(package_format format)
			: _format(format)
		{
		}
		virtual ~package_content_factory() {}

		inline package_format format() const { return _format; }
		virtual package construct(const constr_type& constr, const package& nextPkg) = 0;
		virtual package construct(std::vector<unsigned char> data) = 0;

	private:
		const package_format _format;
	};

	class package_factory
	{
	public:
		enum _empty_factory_tag
		{
			empty_factory
		};

		typedef package_content_factory::constr_type constr_type;
	public:
		package_factory();
		package_factory(_empty_factory_tag);
		virtual ~package_factory();

		void add_format(std::shared_ptr<package_content_factory> formatFactory);

		void default_format(const package_format& format);
		package_format default_format() const;

		package read_package(std::vector<unsigned char> data) const;

		package construct(const constr_type& constr) const;
		package construct(const constr_type& constr, const package& nextPkg) const;
		package construct(const constr_type& constr, const package_format& format) const;

		template<typename T>
		package make_package(const T& val) const
		{
			return construct(_construction_handler(val, serialization::context<>()));
		}

		template<typename T>
		package make_package(const T& val, const package& nextPkg) const
		{
			return construct(_construction_handler(val, serialization::context<>()), nextPkg);
		}

		template<typename T>
		package make_package(const T& val, const package_format& format) const
		{
			return construct(_construction_handler(val, serialization::context<>()), format);
		}

		template<typename T, typename... ContextItem>
		package make_package(const T& val, serialization::context<ContextItem...> context) const
		{
			return construct(_construction_handler(val, context));
		}

		template<typename T, typename... ContextItem>
		package make_package(const T& val, const package& nextPkg, serialization::context<ContextItem...> context) const
		{
			return construct(_construction_handler(val, context), nextPkg);
		}

		template<typename T, typename... ContextItem>
		package make_package(const T& val, const package_format& format, serialization::context<ContextItem...> context) const
		{
			return construct(_construction_handler(val, context), format);
		}

	private:
		template<typename T, typename Context>
		static inline constr_type _construction_handler(const T& val, Context&& context)
		{
			return [&val, context](serialization::generic_sink& sink) mutable
			{
				serialization::serializer<serialization::generic_sink, typename std::decay<Context>::type> serializer(sink, std::move(context));
				serializer << val;
			};
		}

		std::shared_ptr<package_content_factory> _get_factory(package_format format) const;
	private:
		std::shared_ptr<package_content_factory> _defaultFactory;
		std::vector<std::shared_ptr<package_content_factory>> _factories;
	};


}

#include "serialization/serializer.hpp"
#include "serialization/formats/generic_sink.hpp"


#endif