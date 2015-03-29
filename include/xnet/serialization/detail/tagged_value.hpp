#pragma once
#ifndef _XNET_TAGGED_VALUE_HPP
#define _XNET_TAGGED_VALUE_HPP


namespace xnet {
	namespace serialization {
		namespace detail {
			template<typename T>
			class tagged_value
			{
			public:
				tagged_value(const char* name, T& value)
					: _name(name)
					, _value(value)
				{
				}

				const char* name() const { return _name; }
				T& value() { return _value; }
				const T& value() const { return _value; }

			private:
				const char* _name;
				T& _value;
			};
		}
	}
}


#endif