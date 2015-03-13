#pragma once
#ifndef _XNET_DETAIL_FINNALY_HPP
#define _XNET_DETAIL_FINNALY_HPP


#include <functional>

namespace xnet {
	namespace detail {
		class finally
		{
		public:
			template<typename Func>
			finally(Func _func)
				: mFunctor(_func)
			{}
			~finally()
			{
				mFunctor();
			}
			//finally(finally&&) = default;
			finally(const finally&) = delete;
			finally& operator=(const finally&) = delete;
		private:
			std::function<void(void)> mFunctor;
		};
	}
}

#endif