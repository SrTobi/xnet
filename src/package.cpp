#include <boost/assert.hpp>
#include "xnet/package.hpp"

namespace xnet {
	namespace internal {
		class raw_package_content: public package_content
		{
		public:
			raw_package_content(std::vector<unsigned char>&& data)
				: package_content(std::make_shared<std::vector<unsigned char>>(std::move(data)), nullptr)
			{
			}

			virtual package_content_factory* factory() const
			{
				throw std::logic_error("A raw package has no factory!");
			}

			virtual package_format format() const
			{
				return raw_package_format;
			}

			package deserialize(const deshandler_type& des) const
			{
				throw std::logic_error("A raw package can not be deserialized!");
			}

		};
	}

	/************************************** package **************************************/
	package::package()
	{
	}

	package::package(std::nullptr_t)
	{
	}

	package::package(std::shared_ptr<const package_content> content)
		: _content(content)
	{
	}

	package::package(package&& old)
		: _content(std::move(old._content))
	{
	}

	package::package(const package& other)
		: _content(other._content)
	{
	}

	package::~package()
	{
	}

	package::size_type package::size() const
	{
		BOOST_ASSERT(_content);
		return _content->size();
	}

	const std::vector<boost::asio::const_buffer>& package::underlying_data() const
	{
		BOOST_ASSERT(_content);
		return _content->underlying_data();
	}

	package_format package::format() const
	{
		BOOST_ASSERT(_content);
		return _content->format();
	}

	package::operator bool() const
	{
		return _content.operator bool();
	}

	const package_content& package::content() const
	{
		BOOST_ASSERT(_content);
		return *_content;
	}

	bool package::operator!() const
	{
		return !_content;
	}
	xnet::package package::make_raw(std::vector<unsigned char> data)
	{
		return make_package<internal::raw_package_content>(std::move(data));
	}

	
}