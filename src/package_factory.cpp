#include <boost/assert.hpp>
#include "xnet/package_factory.hpp"
#include "xnet/package_formats/xml_package_factory.hpp"

namespace xnet {


	package_factory::package_factory()
	{
		add_format(std::make_shared<xml_package_factory>());
	}

	package_factory::package_factory(_empty_factory_tag)
	{

	}

	package_factory::~package_factory()
	{

	}

	void package_factory::add_format(std::shared_ptr<package_content_factory> formatFactory)
	{
		if (!_defaultFactory)
			_defaultFactory = formatFactory;
		_factories.push_back(std::move(formatFactory));
	}

	void package_factory::default_format(const package_format& format)
	{
		_defaultFactory = _get_factory(format);
		BOOST_ASSERT(_defaultFactory);
	}

	xnet::package_format package_factory::default_format() const
	{
		BOOST_ASSERT(_defaultFactory);
		return _defaultFactory->format();
	}

	xnet::package package_factory::read_package(std::vector<unsigned char> data) const
	{
		throw std::exception("not implemented!");
	}

	xnet::package package_factory::construct(const constr_type& constr) const
	{
		BOOST_ASSERT(_defaultFactory);

		return package(_defaultFactory->construct(constr, nullptr));
	}

	xnet::package package_factory::construct(const constr_type& constr, const package& nextPkg) const
	{
		BOOST_ASSERT_MSG(nextPkg.format() != raw_package_format, "Can chain raw packages!");
		auto factory = nextPkg.content().factory();
		BOOST_ASSERT(factory);
		return factory->construct(constr, nextPkg);

	}

	xnet::package package_factory::construct(const constr_type& constr, const package_format& format) const
	{
		const auto fac = _get_factory(format);

		if (!fac)
		{
			throw std::runtime_error(std::string("Format '") + format.name() + "' is not supported!");
		}

		return package(fac->construct(constr, nullptr));
	}

	std::shared_ptr<package_content_factory> package_factory::_get_factory(package_format format) const
	{
		auto it = std::find_if(_factories.begin(), _factories.end(),
			[format](const std::shared_ptr<package_content_factory>& fac)
		{
			return fac->format() == format;
		});
		return it == _factories.end() ? nullptr : *it;
	}

}