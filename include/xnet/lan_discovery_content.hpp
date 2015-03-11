#pragma once
#ifndef _XNET_LAN_DISCOVERY_CONTENT_HPP
#define _XNET_LAN_DISCOVERY_CONTENT_HPP

#include <string>

namespace xnet {

	class lan_discovery_content
	{
	public:
		lan_discovery_content();
		lan_discovery_content(const std::string& msg);
		~lan_discovery_content();


		template<typename Iter>
		bool deserialize(Iter begin, Iter end)
		{
			_message.assign(begin, end);
			return true;
		}

		std::string serialize() const;

		inline const std::string& message() const { return _message; }

	private:
		std::string _message;
	};



}


#endif