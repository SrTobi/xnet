#pragma once
#ifndef _XNET_SERIALIZATION_DETAIL_SERIALIZER_BASE_HPP
#define _XNET_SERIALIZATION_DETAIL_SERIALIZER_BASE_HPP


namespace xnet {
	namespace serialization {
		namespace detail {
			template<typename Inner>
			class serializer_base
			{
			public:
				Inner& inner() { return static_cast<Inner&>(*this); }
			};
		}
	}
}

#endif