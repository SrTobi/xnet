#pragma once
#ifndef _XNET_SERVICE_SERVICE_DESCRIPTOR_HPP
#define _XNET_SERVICE_SERVICE_DESCRIPTOR_HPP

#include <exception>
#include "../package.hpp"
#include "generic_service.hpp"

namespace xnet {

	namespace service {
		class service_method_descriptor
		{
		public:
			service_method_descriptor(funcid_type id)
				: _id(id)
			{
			}

			virtual void invoke(const std::shared_ptr<generic_service>& service, service_peer& peer, const package& args) const = 0;
			virtual package call(const std::shared_ptr<generic_service>& service, service_peer& peer, returnid_type retId, const package& args) const = 0;

			funcid_type id() const { return _id; }

		private:
			funcid_type _id;
		};

		namespace detail {
			template<int ...>
			struct seq { };

			template<int N, int ...S>
			struct gens : gens<N-1, N-1, S...> { };

			template<int ...S>
			struct gens<0, S...> {
			  typedef seq<S...> type;
			};

			struct void_substitute
			{
				template<typename S>
				void serialize(S& s)
				{
					s.current_type("void");
				}
			};


			template<typename Service, typename Method>
			class specific_service_method_descriptor
				: public service_method_descriptor
			{
			public:
				specific_service_method_descriptor(Method method, funcid_type id)
					: service_method_descriptor(id)
					, _method(method)
				{
				}

				virtual void invoke(const std::shared_ptr<generic_service>& service, service_peer& peer, const package& args) const final override
				{
					try {
						_prepare_call(_method, peer, service, args);
					}
					catch (...)
					{
						// catch all exceptions in services
					}
				}

				virtual package call(const std::shared_ptr<generic_service>& service, service_peer& peer, returnid_type retId, const package& args) const final override
				{
					try {
						const auto return_value = _prepare_call(_method, peer, service, args);
						return peer._make_return_content_package(return_value, retId);
					}
					catch (const call_error& e)
					{
						return peer._make_exception_package(e, retId);
					}
					catch (...)
					{
						return peer._make_internal_exception_package(retId);
					}
				}

			private:
				template<typename Ret, typename... FArgs>
				typename std::conditional<
					std::is_void<Ret>::value,
					void_substitute,
					Ret>
				::type _prepare_call(Ret (Service::*method)(FArgs...), service_peer& peer, const std::shared_ptr<generic_service>& service, const package& arg_pack) const
				{
					std::tuple<typename std::decay<FArgs>::type...> args;
					arg_pack.deserialize(args, serialization::make_context(peer), package::try_next_package);
					auto& sv = dynamic_cast<Service&>(*service);
					return _do_call(method, sv, args, typename gens<sizeof...(FArgs)>::type());
				}

				template<typename Ret, typename... FArgs, typename... Args, int... Sq>
				Ret _do_call(Ret(Service::*method)(FArgs...), Service& service, std::tuple<Args...>& args, seq<Sq...>) const
				{
					return (service.*method)(std::move(std::get<Sq>(args))...);
				}

				template<typename... FArgs, typename... Args, int... Sq>
				void_substitute _do_call(void(Service::*method)(FArgs...), Service& service, std::tuple<Args...>& args, seq<Sq...>) const
				{
					(service.*method)(std::move(std::get<Sq>(args))...);
					return {};
				}

			private:
				Method _method;
			};
		}

		class generic_service_descriptor
		{
		public:
			void invoke(const std::shared_ptr<generic_service>& service, service_peer& peer, funcid_type fundId, const package& args) const
			{
				auto& desc = _get_method_desc(fundId);
				desc.invoke(service, peer, args);
			}

			package call(const std::shared_ptr<generic_service>& service, service_peer& peer, funcid_type fundId, returnid_type retId, const package& args) const
			{
				auto& desc = _get_method_desc(fundId);
				return desc.call(service, peer, retId, args);
			}

			const std::string& checksum() const
			{
				return _checksum;
			}
		protected:
			void _gen_checksum()
			{
				if (_methods.empty())
					throw std::logic_error("No functions registered in service!");
			}

		private:
			const service_method_descriptor& _get_method_desc(funcid_type funcId) const
			{
				if (funcId > _methods.size() || funcId <= 0)
				{
					throw std::runtime_error("Requested function does not exist in this service!");
				}
				return *(_methods[funcId - 1]);
			}

		protected:
			std::vector<std::shared_ptr<service_method_descriptor>> _methods;
			std::string _checksum;
		};

		template<typename Service>
		class service_descriptor
			: public generic_service_descriptor
		{
			static_assert(is_service<Service>::value, "Service must be a service!");
		private:
			template<typename FuncDecl>
			struct func_storage
			{
				typedef detail::specific_service_method_descriptor<Service, FuncDecl> mdesc_type;
				typedef std::pair<FuncDecl, std::shared_ptr<mdesc_type>> func_mapping_item;
				typedef std::vector<func_mapping_item> func_mapping;

				static std::shared_ptr<service_method_descriptor> add_method(const std::string& name, FuncDecl method, funcid_type id)
				{
					static func_mapping funcMapping;

					funcMapping.emplace_back(method, std::make_shared<mdesc_type>(method, id));

					/* TODOD:
					 *if (!res.second)
					{
						throw std::logic_error("An attempt to register a function failed, because the function is already registered!");
					}/*/

					_funcMapping = &funcMapping;
					return funcMapping.back().second;
				}

				static const mdesc_type& resolve_method(FuncDecl method)
				{
					auto it = std::find_if(_funcMapping->begin(), _funcMapping->end(), [method](const func_mapping_item& left)
					{
						return left.first == method;
					});

					if (it == _funcMapping->end())
					{
						throw std::logic_error("Requested method is not registered!");
					}

					return *(it->second);
				}

			private:
				static const func_mapping* _funcMapping;
			};
		public:
			template<typename InitHandler>
			service_descriptor(const std::string& name, const InitHandler& handler)
			{
				handler(*this);
				_gen_checksum();
			}

			template<typename Ret, typename... Args>
			service_descriptor& add_method(const std::string& name, Ret (Service::*method)(Args...))
			{
				auto m = func_storage<Ret(Service::*)(Args...)>::add_method(name, method, _methods.size() + 1);
				_methods.push_back(m);
				assert(m->id() == _methods.size());
				return *this;
			}

			template<typename Ret, typename... Args>
			const service_method_descriptor& resolve_method(Ret(Service::*method)(Args...)) const
			{
				return func_storage<Ret(Service::*)(Args...)>::resolve_method(method);
			}
		};
		template<typename Service>
		template<typename FuncDecl>
		typename const service_descriptor<Service>::func_storage<FuncDecl>::func_mapping*
			service_descriptor<Service>::func_storage<FuncDecl>::_funcMapping;


		namespace detail {
			template<typename Service>
			struct service_descriptor_getter
			{
				static_assert(is_service<Service>::value, "Service must be a service!");
				inline static const service_descriptor<Service>& get();
			};
		}

		template<typename Service>
		const service_descriptor<Service>& get_descriptor()
		{
			static_assert(is_service<Service>::value, "Service must be a service!");
			return detail::service_descriptor_getter<Service>::get();
		}

#define XNET_IMPLEMENT_SERVICE_DESCRIPTOR_NAMED(_nspace, _service, _name, _desk)	\
		void _Xnet_init_service_##_service##_descriptor(::xnet::service::service_descriptor<_nspace::_service>& _desk); \
		namespace xnet_detail { const ::xnet::service::service_descriptor<_nspace::_service> _Xnet_##_service##_descriptor(_name, \
		[](::xnet::service::service_descriptor<_nspace::_service>& desk){ _Xnet_init_service_##_service##_descriptor(desk); }); }\
		namespace xnet {namespace service { const service_descriptor<_nspace::_service>& detail::service_descriptor_getter<_nspace::_service>::get() { return ::xnet_detail::_Xnet_##_service##_descriptor;}\
		const generic_service_descriptor& abstract_service<_nspace::_service>::_descriptor() const {return ::xnet_detail::_Xnet_##_service##_descriptor;}}} \
		void _Xnet_init_service_##_service##_descriptor(::xnet::service::service_descriptor<_nspace::_service>& _desk)

#define XNET_IMPLEMENT_SERVICE_DESCRIPTOR(_nspace, _service, _desk)	\
			XNET_IMPLEMENT_SERVICE_DESCRIPTOR_NAMED(_nspace, _service, #_service, _desk)
	}
}


#endif