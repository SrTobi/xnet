#include <testx/testx.hpp>
#include <boost/noncopyable.hpp>
#include <testx/wchar_tools.hpp>
#include <xnet/service/service_peer.hpp>



namespace service_tests {
	using namespace xnet::service;

	class onlymovable
		: public boost::noncopyable
	{
	public:
		onlymovable()
		{}
		onlymovable(onlymovable&&)
		{}

		template<typename S>
		void serialize(S& s)
		{
			s.current_type("onlymovable");
		}
	};

	enum class service_events
	{
		called__void_test = 0,
		called__void_params_test,
		called__int_test,
		called__int_params_test,
		called__noncopyable_test,
		returned,
		_num_events
	};


	inline std::ostream& operator<<(std::ostream& out, const service_events& value)
	{
		std::array<std::string, (int)service_events::_num_events> evt_names =
		{
			"called__void_test",
			"called_void_params_test",
			"called__int_test",
			"called__int_params_test",
			"called__noncopyable_test",
			"returned"
		};
		out << evt_names[(int)value];
		return out;
	}



	typedef testx::MockObserver<service_events> mock_observer;

	XNET_SERVICE(test_service)
	{
	public:
		test_service(mock_observer observer)
			: _observer(observer)
		{
		}

	public:
		void void_test()
		{
			_observer.expect(service_events::called__void_test);
		}

		void void_params_test(int i)
		{
			_observer.expect(service_events::called__void_params_test);
			BOOST_CHECK_EQUAL(i, 666);
		}

		int int_test()
		{
			_observer.expect(service_events::called__int_test);
			return 777;
		}

		int int_params_test(std::string str)
		{
			_observer.expect(service_events::called__int_params_test);
			BOOST_CHECK_EQUAL(str, "devil");
			return 888;
		}
 
		onlymovable noncopyable_test(onlymovable)
		{
			_observer.expect(service_events::called__noncopyable_test);
			return onlymovable();
		}

	private:
		mock_observer _observer;
	};

	class service_tester
	{
	public:
		service_tester()
			: _client(&_factory)
			, _server(&_factory)
		{
			_server.add_service("default",  std::make_shared<test_service>(_observer));
		}

		void check_invoke__void_test()
		{
			_observer.set()
				<< service_events::called__void_test;

			auto pack = _client.make_invokation("default", &test_service::void_test);
			check_invoke(pack);
		}

		void check_call__void_test()
		{
			_observer.set()
				<< service_events::called__void_test
				<< service_events::returned;

			auto pack = _client.make_call("default", &test_service::void_test, [this](){
				_observer.expect(service_events::returned);
			}, _expect_no_error_func);
			check_call(pack);
		}

		void check_invoke__void_params_test()
		{
			_observer.set()
				<< service_events::called__void_params_test;

			auto pack = _client.make_invokation("default", &test_service::void_params_test, 666);
			check_invoke(pack);
		}

		void check_call__void_params_test()
		{
			_observer.set()
				<< service_events::called__void_params_test
				<< service_events::returned;

			auto pack = _client.make_call("default", &test_service::void_params_test, [this](){
				_observer.expect(service_events::returned);
			}, _expect_no_error_func,
			666);
			check_call(pack);
		}

		void check_invoke__int_test()
		{
			_observer.set()
				<< service_events::called__int_test;

			auto pack = _client.make_invokation("default", &test_service::int_test);
			check_invoke(pack);
		}

		void check_call__int_test()
		{
			_observer.set()
				<< service_events::called__int_test
				<< service_events::returned;

			auto pack = _client.make_call("default", &test_service::int_test, [this](int i){
				_observer.expect(service_events::returned);
				BOOST_CHECK_EQUAL(i, 777);
			}, _expect_no_error_func);
			check_call(pack);
		}

		void check_invoke__int_params_test()
		{
			_observer.set()
				<< service_events::called__int_params_test;

			auto pack = _client.make_invokation("default", &test_service::int_params_test, "devil");
			check_invoke(pack);
		}

		void check_call__int_params_test()
		{
			_observer.set()
				<< service_events::called__int_params_test
				<< service_events::returned;

			auto pack = _client.make_call("default", &test_service::int_params_test, [this](int i){
				_observer.expect(service_events::returned);
				BOOST_CHECK_EQUAL(i, 888);
			}, _expect_no_error_func,
			"devil");
			check_call(pack);
		}

		void check_invoke__noncopyable_test()
		{
			_observer.set()
				<< service_events::called__noncopyable_test;

			auto pack = _client.make_invokation("default", &test_service::noncopyable_test, onlymovable());
			check_invoke(pack);
		}

		void check_call__noncopyable_test()
		{
			_observer.set()
				<< service_events::called__noncopyable_test
				<< service_events::returned;

			auto pack = _client.make_call("default", &test_service::noncopyable_test, [this](onlymovable){
				_observer.expect(service_events::returned);
			}, _expect_no_error_func,
			onlymovable());
			check_call(pack);
		}

	private:
		void check_invoke(const xnet::package& pack)
		{
			BOOST_CHECK(pack);
			auto ret_pack = _server.process_package(pack);
			BOOST_CHECK(!ret_pack);
		}

		void check_call(const xnet::package& pack)
		{
			BOOST_CHECK(pack);
			auto ret_pack = _server.process_package(pack);
			BOOST_CHECK(ret_pack);
			auto null_pack = _client.process_package(ret_pack);
			BOOST_CHECK(!null_pack);
		}

	private:
		xnet::package_factory _factory;
		service_peer _client;
		service_peer _server;
		mock_observer _observer;

		std::function<void(const xnet::service::call_error&)> _expect_no_error_func = [](const xnet::service::call_error&)
		{
			BOOST_ERROR("no exception expected");
		};
	};


	TESTX_START_FIXTURE_TEST(service_tester)
		TESTX_FIXTURE_TEST(check_invoke__int_test);
		TESTX_FIXTURE_TEST(check_call__int_test);

		TESTX_FIXTURE_TEST(check_invoke__void_test);
		TESTX_FIXTURE_TEST(check_call__void_test);

		TESTX_FIXTURE_TEST(check_invoke__void_params_test);
		TESTX_FIXTURE_TEST(check_call__void_params_test);

		TESTX_FIXTURE_TEST(check_invoke__int_params_test);
		TESTX_FIXTURE_TEST(check_call__int_params_test);

		TESTX_FIXTURE_TEST(check_invoke__noncopyable_test);
		TESTX_FIXTURE_TEST(check_call__noncopyable_test);
	TESTX_END_FIXTURE_TEST();
}


XNET_IMPLEMENT_SERVICE_DESCRIPTOR(service_tests, test_service, desc)
{
	using namespace service_tests;
	desc.add_method("void_test", &test_service::void_test);
	desc.add_method("void_test", &test_service::void_params_test);
	desc.add_method("int_test", &test_service::int_test);
	desc.add_method("int_params_test", &test_service::int_params_test);
	desc.add_method("noncopyable_test", &test_service::noncopyable_test);
}

