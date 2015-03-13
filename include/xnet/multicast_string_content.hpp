#pragma once
#ifndef _XNET_MULTICAST_STRING_CONTENT_HPP
#define _XNET_MULTICAST_STRING_CONTENT_HPP

#include <string>

namespace xnet {

	class multicast_string_content
	{
	public:
		multicast_string_content();
		multicast_string_content(const std::string& msg);
		~multicast_string_content();


		template<typename Iter>
		bool deserialize(Iter begin, Iter end)
		{
			_message.assign(begin, end);
			return true;
		}

		std::string serialize() const;

		inline const std::string& value() const { return _message; }

	private:
		std::string _message;
	};



}


#endif