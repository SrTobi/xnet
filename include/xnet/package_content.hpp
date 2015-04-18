#pragma once
#ifndef _XNET_PACKAGE_CONTENT_HPP
#define _XNET_PACKAGE_CONTENT_HPP

#include <memory>
#include <vector>
#include <boost/asio/buffer.hpp>
#include "package_format.hpp"

namespace xnet {

	
	class package_content_factory;
	class package;

	namespace serialization {
		class generic_source;
	}

	class package_content
	{
	public:
		typedef std::function<void(serialization::generic_source&)> deshandler_type;
		typedef unsigned int size_type;
	public:
		virtual ~package_content();

		virtual package_content_factory* factory() const = 0;
		virtual package_format format() const = 0;
		size_type size() const;

		virtual package deserialize(const deshandler_type& des) const = 0;

		const std::vector<boost::asio::const_buffer>& underlying_data() const;

	protected:
		package_content(
			std::shared_ptr<const std::vector<unsigned char>> content,
			const package& nextPkg);

	private:
		const size_type _size;
		const std::shared_ptr<const std::vector<unsigned char>> _content;
		const std::vector<boost::asio::const_buffer> _buffers;
	};



}


#endif