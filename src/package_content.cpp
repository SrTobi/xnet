#include <boost/assert.hpp>
#include "xnet/package_content.hpp"
#include "xnet/package_factory.hpp"

namespace xnet {

	static inline package_content::size_type _calc_package_content_size(const std::shared_ptr<const std::vector<unsigned char>>& content, const package& nextPkg)
	{
		BOOST_ASSERT(content);
		return content->size() + (nextPkg ? nextPkg.size() : 0);
	}

	static inline std::vector<boost::asio::const_buffer> _build_buffers(const std::shared_ptr<const std::vector<unsigned char>> content, const package& nextPkg)
	{
		std::vector<boost::asio::const_buffer> buffers
			{
				boost::asio::const_buffer(content->data(), content->size())
			};

		if (nextPkg)
		{
			const auto& other = nextPkg.underlying_data();
			buffers.insert(buffers.end(), other.begin(), other.end());
		}

		return buffers;
	}

	package_content::package_content(std::shared_ptr<const std::vector<unsigned char>> content, const package& nextPkg)
		: _size(_calc_package_content_size(content, nextPkg))
		, _content(std::move(content))
		, _buffers(_build_buffers(_content, nextPkg))
	{
		BOOST_ASSERT(_content);
	}

	package_content::~package_content()
	{
	}

	package_content::size_type package_content::size() const
	{
		return _size;
	}

	const std::vector<boost::asio::const_buffer>& package_content::underlying_data() const
	{
		return _buffers;
	}

}