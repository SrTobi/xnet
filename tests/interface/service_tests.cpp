#include <testx/testx.hpp>
#include <boost/noncopyable.hpp>
#include <testx/wchar_tools.hpp>
#include <xnet/service/service_peer.hpp>
#include <xnet/service/remote_service.hpp>


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
		called__service_test,
		called__throw_user_error,
		called__throw_std_error,
		client_process_package,
		server_process_package,
		get_test_service,
		returned,
		user_error,
		std_error,
		destruct__test_service,
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
			"called__service_test",
			"called__throw_user_error",
			"called__throw_std_error",
			"client_process_package",
			"server_process_package",
			"get_test_service",
			"returned",
			"user_error",
			"std_error",
			"destruct__test_service"
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
			++_count;
		}

		~test_service()
		{
			_observer.expect(service_events::destruct__test_service);
			--_count;
		}

		static int Count()
		{
			return _count;
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

		remote_service<test_service> get_test_service()
		{
			_observer.expect(service_events::get_test_service);
			return std::make_shared<test_service>(_observer);
		}

		remote_service<test_service> service_test(remote_service<test_service> other)
		{
			_observer.expect(service_events::called__service_test);
			return std::make_shared<test_service>(_observer);
		}

		void throw_user_error()
		{
			_observer.expect(service_events::called__throw_user_error);
			throw xnet::service::call_error("user defined error");
		}

		void throw_std_error()
		{
			_observer.expect(service_events::called__throw_std_error);
			throw std::exception("...internal error something");
		}

	private:
		mock_observer _observer;
		static int _count;
	};
	int test_service::_count = 0;

	class service_tester
	{
	public:
		service_tester()
			: _client(&_factory, [this](xnet::package p) { BOOST_CHECK(p); _observer.expect(service_events::server_process_package); _server.process_package(p); })
			, _server(&_factory, [this](xnet::package p) { BOOST_CHECK(p); _observer.expect(service_events::client_process_package); _client.process_package(p); })
		{
			_server.add_service("default",  std::make_shared<test_service>(_observer));
		}

		~service_tester()
		{
			_observer.set()
				<< service_events::destruct__test_service;
			_server.remove_service("default");
			BOOST_CHECK_EQUAL(test_service::Count(), 0);
			BOOST_CHECK_EQUAL(_client.open_return_slots(), 0);
			BOOST_CHECK_EQUAL(_server.open_return_slots(), 0);
		}


		/************************************** static invokes/calls **************************************/
		void check_invoke__void_test()
		{
			_observer.set()
				<< service_events::server_process_package
				<< service_events::called__void_test;

			_client.invoke("default", &test_service::void_test);
		}

		void check_call__void_test()
		{
			_observer.set()
				<< service_events::server_process_package
				<< service_events::called__void_test
				<< service_events::client_process_package
				<< service_events::returned;

			_client.call("default", &test_service::void_test, [this](){
				_observer.expect(service_events::returned);
			}, _expect_no_error_func);
		}

		void check_invoke__void_params_test()
		{
			_observer.set()
				<< service_events::server_process_package
				<< service_events::called__void_params_test;

			_client.invoke("default", &test_service::void_params_test, 666);
		}

		void check_call__void_params_test()
		{
			_observer.set()
				<< service_events::server_process_package
				<< service_events::called__void_params_test
				<< service_events::client_process_package
				<< service_events::returned;

			_client.call("default", &test_service::void_params_test, [this](){
				_observer.expect(service_events::returned);
			}, _expect_no_error_func,
			666);
		}

		void check_invoke__int_test()
		{
			_observer.set()
				<< service_events::server_process_package
				<< service_events::called__int_test;

			_client.invoke("default", &test_service::int_test);
		}

		void check_call__int_test()
		{
			_observer.set()
				<< service_events::server_process_package
				<< service_events::called__int_test
				<< service_events::client_process_package
				<< service_events::returned;

			_client.call("default", &test_service::int_test, [this](int i){
				_observer.expect(service_events::returned);
				BOOST_CHECK_EQUAL(i, 777);
			}, _expect_no_error_func);
		}

		void check_invoke__int_params_test()
		{
			_observer.set()
				<< service_events::server_process_package
				<< service_events::called__int_params_test;

			_client.invoke("default", &test_service::int_params_test, "devil");
		}

		void check_call__int_params_test()
		{
			_observer.set()
				<< service_events::server_process_package
				<< service_events::called__int_params_test
				<< service_events::client_process_package
				<< service_events::returned;

			_client.call("default", &test_service::int_params_test, [this](int i){
				_observer.expect(service_events::returned);
				BOOST_CHECK_EQUAL(i, 888);
			}, _expect_no_error_func,
			"devil");
		}

		void check_invoke__noncopyable_test()
		{
			_observer.set()
				<< service_events::server_process_package
				<< service_events::called__noncopyable_test;

			_client.invoke("default", &test_service::noncopyable_test, onlymovable());
		}

		void check_call__noncopyable_test()
		{
			_observer.set()
				<< service_events::server_process_package
				<< service_events::called__noncopyable_test
				<< service_events::client_process_package
				<< service_events::returned;

			_client.call("default", &test_service::noncopyable_test, [this](onlymovable){
				_observer.expect(service_events::returned);
			}, _expect_no_error_func,
			onlymovable());
		}

		void check_invoke__service_test()
		{
			_observer.set()
				<< service_events::server_process_package
				<< service_events::called__service_test
				<< service_events::client_process_package
				<< service_events::destruct__test_service;

			auto sv = _server.get_service<test_service>("default");
			BOOST_CHECK(sv);

			_client.invoke("default", &test_service::service_test, sv);
		}

		void check_call__service_test()
		{
			_observer.set()
				<< service_events::server_process_package
				<< service_events::called__service_test
				<< service_events::client_process_package
				//<< service_events::destruct__test_service; <-- because we used the default service!
				<< service_events::client_process_package
				<< service_events::returned
				<< service_events::server_process_package
				<< service_events::destruct__test_service;


			auto sv = _server.get_service<test_service>("default");
			BOOST_CHECK(sv);

			_client.call("default", &test_service::service_test, [this](remote_service<test_service> s){
				_observer.expect(service_events::returned);
			}, _expect_no_error_func,
			sv);
		}

		void check_invoke__throw_user_error()
		{
			_observer.set()
				<< service_events::server_process_package
				<< service_events::called__throw_user_error;

			_client.invoke("default", &test_service::throw_user_error);
		}

		void check_call__throw_user_error()
		{
			_observer.set()
				<< service_events::server_process_package
				<< service_events::called__throw_user_error
				<< service_events::client_process_package
				<< service_events::user_error;

			_client.call("default", &test_service::throw_user_error, [](){}, _expect_user_error_func);
		}

		void check_invoke__throw_std_error()
		{
			_observer.set()
				<< service_events::server_process_package
				<< service_events::called__throw_std_error;

			_client.invoke("default", &test_service::throw_std_error);
		}

		void check_call__throw_std_error()
		{
			_observer.set()
				<< service_events::server_process_package
				<< service_events::called__throw_std_error
				<< service_events::client_process_package
				<< service_events::std_error;

			_client.call("default", &test_service::throw_std_error, [](){}, _expect_std_error_func);
		}

		/************************************** dynamic invokes/calls **************************************/
		void check_dyn_invoke__void_test()
		{
			auto s = dyn_get();
			_observer.set()
				<< service_events::server_process_package
				<< service_events::called__void_test;

			s.invoke(&test_service::void_test);

			dyn_end();
		}

		void check_dyn_call__void_test()
		{
			auto s = dyn_get();
			_observer.set()
				<< service_events::server_process_package
				<< service_events::called__void_test
				<< service_events::client_process_package
				<< service_events::returned;

			s.call(&test_service::void_test, [this](){
				_observer.expect(service_events::returned);
			}, _expect_no_error_func);

			dyn_end();
		}

		void check_dyn_invoke__void_params_test()
		{
			auto s = dyn_get();
			_observer.set()
				<< service_events::server_process_package
				<< service_events::called__void_params_test;

			s.invoke(&test_service::void_params_test, 666);
			dyn_end();
		}

		void check_dyn_call__void_params_test()
		{
			auto s = dyn_get();
			_observer.set()
				<< service_events::server_process_package
				<< service_events::called__void_params_test
				<< service_events::client_process_package
				<< service_events::returned;

			s.call(&test_service::void_params_test, [this](){
				_observer.expect(service_events::returned);
			}, _expect_no_error_func,
			666);
			dyn_end();
		}

		void check_dyn_invoke__int_test()
		{
			auto s = dyn_get();
			_observer.set()
				<< service_events::server_process_package
				<< service_events::called__int_test;

			s.invoke(&test_service::int_test);
			dyn_end();
		}

		void check_dyn_call__int_test()
		{
			auto s = dyn_get();
			_observer.set()
				<< service_events::server_process_package
				<< service_events::called__int_test
				<< service_events::client_process_package
				<< service_events::returned;

			s.call(&test_service::int_test, [this](int i){
				_observer.expect(service_events::returned);
				BOOST_CHECK_EQUAL(i, 777);
			}, _expect_no_error_func);
			dyn_end();
		}

		void check_dyn_invoke__int_params_test()
		{
			auto s = dyn_get();
			_observer.set()
				<< service_events::server_process_package
				<< service_events::called__int_params_test;

			s.invoke(&test_service::int_params_test, "devil");
			dyn_end();
		}

		void check_dyn_call__int_params_test()
		{
			auto s = dyn_get();
			_observer.set()
				<< service_events::server_process_package
				<< service_events::called__int_params_test
				<< service_events::client_process_package
				<< service_events::returned;

			s.call(&test_service::int_params_test, [this](int i){
				_observer.expect(service_events::returned);
				BOOST_CHECK_EQUAL(i, 888);
			}, _expect_no_error_func,
			"devil");
			dyn_end();
		}

		void check_dyn_invoke__noncopyable_test()
		{
			auto s = dyn_get();
			_observer.set()
				<< service_events::server_process_package
				<< service_events::called__noncopyable_test;

			s.invoke(&test_service::noncopyable_test, onlymovable());
			dyn_end();
		}

		void check_dyn_call__noncopyable_test()
		{
			auto s = dyn_get();
			_observer.set()
				<< service_events::server_process_package
				<< service_events::called__noncopyable_test
				<< service_events::client_process_package
				<< service_events::returned;

			s.call(&test_service::noncopyable_test, [this](onlymovable){
				_observer.expect(service_events::returned);
			}, _expect_no_error_func,
			onlymovable());
			dyn_end();
		}

		void check_dyn_invoke__service_test()
		{
			auto s = dyn_get();
			_observer.set()
				<< service_events::server_process_package
				<< service_events::called__service_test
				<< service_events::client_process_package
				<< service_events::destruct__test_service;

			auto sv = _server.get_service<test_service>("default");
			BOOST_CHECK(sv);

			s.invoke(&test_service::service_test, sv);
			dyn_end();
		}

		void check_dyn_call__service_test()
		{
			auto s = dyn_get();
			_observer.set()
				<< service_events::server_process_package
				<< service_events::called__service_test
				<< service_events::client_process_package
				//<< service_events::destruct__test_service; <-- because we used the default service!
				<< service_events::client_process_package
				<< service_events::returned
				<< service_events::server_process_package
				<< service_events::destruct__test_service;

			auto sv = _server.get_service<test_service>("default");
			BOOST_CHECK(sv);

			s.call(&test_service::service_test, [this](remote_service<test_service> s){
				_observer.expect(service_events::returned);
			}, _expect_no_error_func,
			sv);
			dyn_end();
		}

		void check_dyn_invoke__throw_user_error()
		{
			auto s = dyn_get();
			_observer.set()
				<< service_events::server_process_package
				<< service_events::called__throw_user_error;

			s.invoke(&test_service::throw_user_error);
			dyn_end();
		}

		void check_dyn_call__throw_user_error()
		{
			auto s = dyn_get();
			_observer.set()
				<< service_events::server_process_package
				<< service_events::called__throw_user_error
				<< service_events::client_process_package
				<< service_events::user_error;

			s.call(&test_service::throw_user_error, [](){}, _expect_user_error_func);
			dyn_end();
		}

		void check_dyn_invoke__throw_std_error()
		{
			auto s = dyn_get();
			_observer.set()
				<< service_events::server_process_package
				<< service_events::called__throw_std_error;

			s.invoke(&test_service::throw_std_error);
			dyn_end();
		}

		void check_dyn_call__throw_std_error()
		{
			auto s = dyn_get();
			_observer.set()
				<< service_events::server_process_package
				<< service_events::called__throw_std_error
				<< service_events::client_process_package
				<< service_events::std_error;

			s.call(&test_service::throw_std_error, [](){}, _expect_std_error_func);
			dyn_end();
		}
	private:
		remote_service<test_service> dyn_get()
		{
			remote_service<test_service> service;
			BOOST_CHECK(!service);
			_observer.set()
				<< service_events::server_process_package
				<< service_events::get_test_service
				<< service_events::client_process_package
				<< service_events::returned;

			_client.call("default", &test_service::get_test_service, [&](remote_service<test_service> s) {
				_observer.expect(service_events::returned);
				service = s;
			}, _expect_no_error_func);

			BOOST_CHECK(service);

			return service;
		}

		void dyn_end()
		{
			_observer.set()
				<< service_events::server_process_package
				<< service_events::destruct__test_service;
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

		std::function<void(const xnet::service::call_error&)> _expect_std_error_func = [&](const xnet::service::call_error& e)
		{
			_observer.expect(service_events::std_error);
			BOOST_CHECK_EQUAL(e.what(), std::string("internal service error"));
		};

		std::function<void(const xnet::service::call_error&)> _expect_user_error_func = [&](const xnet::service::call_error& e)
		{
			_observer.expect(service_events::user_error);
			BOOST_CHECK_EQUAL(e.what(), std::string("user defined error"));
		};
	};


	TESTX_START_FIXTURE_TEST(service_tester)
		TESTX_FIXTURE_TEST(check_invoke__void_test);
		TESTX_FIXTURE_TEST(check_call__void_test);

		TESTX_FIXTURE_TEST(check_invoke__void_params_test);
		TESTX_FIXTURE_TEST(check_call__void_params_test);

		TESTX_FIXTURE_TEST(check_invoke__int_test);
		TESTX_FIXTURE_TEST(check_call__int_test);

		TESTX_FIXTURE_TEST(check_invoke__int_params_test);
		TESTX_FIXTURE_TEST(check_call__int_params_test);

		TESTX_FIXTURE_TEST(check_invoke__noncopyable_test);
		TESTX_FIXTURE_TEST(check_call__noncopyable_test);

		TESTX_FIXTURE_TEST(check_invoke__service_test);
		TESTX_FIXTURE_TEST(check_call__service_test);

		TESTX_FIXTURE_TEST(check_invoke__throw_user_error);
		TESTX_FIXTURE_TEST(check_call__throw_user_error);

		TESTX_FIXTURE_TEST(check_invoke__throw_std_error);
		TESTX_FIXTURE_TEST(check_call__throw_std_error);


		TESTX_FIXTURE_TEST(check_dyn_invoke__void_test);
		TESTX_FIXTURE_TEST(check_dyn_call__void_test);

		TESTX_FIXTURE_TEST(check_dyn_invoke__void_params_test);
		TESTX_FIXTURE_TEST(check_dyn_call__void_params_test);

		TESTX_FIXTURE_TEST(check_dyn_invoke__int_test);
		TESTX_FIXTURE_TEST(check_dyn_call__int_test);

		TESTX_FIXTURE_TEST(check_dyn_invoke__int_params_test);
		TESTX_FIXTURE_TEST(check_dyn_call__int_params_test);

		TESTX_FIXTURE_TEST(check_dyn_invoke__noncopyable_test);
		TESTX_FIXTURE_TEST(check_dyn_call__noncopyable_test);

		TESTX_FIXTURE_TEST(check_dyn_invoke__service_test);
		TESTX_FIXTURE_TEST(check_dyn_call__service_test);

		TESTX_FIXTURE_TEST(check_dyn_invoke__throw_user_error);
		TESTX_FIXTURE_TEST(check_dyn_call__throw_user_error);

		TESTX_FIXTURE_TEST(check_dyn_invoke__throw_std_error);
		TESTX_FIXTURE_TEST(check_dyn_call__throw_std_error);
	TESTX_END_FIXTURE_TEST();
}


XNET_IMPLEMENT_SERVICE_DESCRIPTOR(service_tests, test_service, desc)
{
	using namespace service_tests;
	desc.add_method("void_test", &test_service::void_test);
	desc.add_method("void_test", &test_service::void_params_test);
	desc.add_method("int_test", &test_service::int_test);
	desc.add_method("int_params_test", &test_service::int_params_test);
	desc.add_method("get_test_service", &test_service::get_test_service);
	desc.add_method("noncopyable_test", &test_service::noncopyable_test);
	desc.add_method("service_test", &test_service::service_test);
	desc.add_method("throw_user_error", &test_service::throw_user_error);
	desc.add_method("throw_std_error", &test_service::throw_std_error);
}

