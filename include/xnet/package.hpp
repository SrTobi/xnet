#pragma once
#ifndef _XNET_PACKAGE_HPP
#define _XNET_PACKAGE_HPP

#include "serialization/context.hpp"
#include "serialization/serialization_error.hpp"
#include "package_content.hpp"

namespace xnet {

	class package
	{
		friend class package_factory;
	public:
		typedef package_content::size_type size_type;
		enum next_package_option
		{
			request_next_package,
			try_next_package
		};
	public:
		package();
		package(std::nullptr_t);
		package(package&& old);
		package(const package& other);
		virtual ~package();

		template<typename T>
		package deserialize(T& out, next_package_option nextOpt = request_next_package) const
		{
			return deserialize(out, serialization::context<>(), nextOpt);
		}

		template<typename T, typename... ContextItems>
		package deserialize(T& out, serialization::context<ContextItems...> context, next_package_option nextOpt = request_next_package) const
		{
			package next = _content->deserialize([&out, context](serialization::generic_source& source)
			{
				serialization::deserializer<serialization::generic_source, serialization::context<ContextItems...>> deserializer(source, context);
				deserializer >> out;
			});

			if (nextOpt == request_next_package && !next)
			{
				throw serialization_error("Requested a next package, but there is no further package!");
			}

			return next;
		}

		size_type size() const;
		const std::vector<boost::asio::const_buffer>& underlying_data() const;
		package_format format() const;
		
		operator bool() const;
		bool operator !() const;

		const package_content& content() const;

		template<typename PackageType, typename... Args>
		static package make_package(Args&&... args)
		{
			static_assert(std::is_base_of<package_content, PackageType>::value, "package_content must be base of PackageType!");
			return std::make_shared<PackageType>(std::forward<Args>(args)...);
		}

		static package make_raw(std::vector<unsigned char> data);
	private:
		package(std::shared_ptr<const package_content> content);
	private:
		std::shared_ptr<const package_content> _content;
	};

	template<typename T>
	T unpack_package(const package& pack)
	{
		T out;
		pack.deserialize(out);
		return out;
	}

	template<typename T>
	T unpack_package(const package& pack, package& nextPkg, package::next_package_option nextOpt= package::request_next_package)
	{
		T out;
		nextPkg = pack.deserialize(out, nextOpt);
		return out;
	}
}

#include "serialization/formats/generic_source.hpp"
#include "serialization/deserializer.hpp"


#endif