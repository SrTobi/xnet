#pragma once
#ifndef _XNET_PACKAGE_HPP
#define _XNET_PACKAGE_HPP

#include <vector>
#include "serialization/deserializer.hpp"

namespace xnet {


	class package
	{
	public:
		virtual ~package();

		template<typename T>
		void deserialze(T& _out)
		{
			
		}

	private:
	};








}


#endif