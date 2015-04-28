#include "testx/testx.hpp"
#include <xnet/detail/variadic_utils.hpp>

namespace variadic_utils_test {
	using namespace xnet::detail;
	TESTX_AUTO_TEST_CASE(and_test)
	{

		static_assert(variadic_and<true, true>::value, "should be true!");
		static_assert(!variadic_and<true, false>::value, "should be false!");
		static_assert(!variadic_and<true, false, true>::value, "should be false!");
		static_assert(variadic_and<true, true, true>::value, "should be true!");
	}


	TESTX_AUTO_TEST_CASE(or_test)
	{

		static_assert(variadic_or<true, true>::value, "should be true!");
		static_assert(variadic_or<true, false>::value, "should be true!");
		static_assert(variadic_or<true, false, true>::value, "should be true!");
		static_assert(variadic_or<true, true, true>::value, "should be true!");

		static_assert(!variadic_or<false, false>::value, "should be false!");
		static_assert(!variadic_or<false, false, false>::value, "should be false!");
	}
}