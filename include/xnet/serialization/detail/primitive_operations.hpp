#pragma once
#ifndef _XNET_SERIALIZATION_DETAIL_PRIMITIVE_OPERATIONS_HPP
#define _XNET_SERIALIZATION_DETAIL_PRIMITIVE_OPERATIONS_HPP

#define XNET_DETAIL_PRIMITIVE_GENERIC_LOAD_OPERATIONS(funcdef, ref, preconst, body, ...) \
			/* bool */\
			funcdef(bool ref __VA_ARGS__) body; \
			/* char types */ \
			funcdef(signed char ref __VA_ARGS__) body;	\
			funcdef(unsigned char ref __VA_ARGS__) body; \
			funcdef(wchar_t ref __VA_ARGS__) body; \
			/* short */\
			funcdef(signed short ref __VA_ARGS__) body; \
			funcdef(unsigned short ref __VA_ARGS__) body; \
			/* int  */\
			funcdef(signed int ref __VA_ARGS__) body; \
			funcdef(unsigned int ref __VA_ARGS__) body; \
			/* long*/\
			funcdef(signed long ref __VA_ARGS__) body; \
			funcdef(unsigned long ref __VA_ARGS__) body; \
			/* floating points */\
			funcdef(float ref __VA_ARGS__) body; \
			funcdef(double ref __VA_ARGS__) body; \
			funcdef(long double ref __VA_ARGS__) body; \
			/* string types */\
			funcdef(preconst std::string& __VA_ARGS__) body; \
			funcdef(preconst std::wstring& __VA_ARGS__) body; \
			funcdef(preconst std::u16string& __VA_ARGS__) body; \
			funcdef(preconst std::u32string& __VA_ARGS__) body;

#define XNET_DETAIL_PRIMITIVE_LOAD_OPERATIONS(funcdef, body, ...) \
	XNET_DETAIL_PRIMITIVE_GENERIC_LOAD_OPERATIONS(funcdef, &, , body, __VA_ARGS__)

#define XNET_DETAIL_PRIMITIVE_SAVE_OPERATIONS(funcdef, body, ...) \
			XNET_DETAIL_PRIMITIVE_GENERIC_LOAD_OPERATIONS(funcdef, , const, body, __VA_ARGS__) \
			funcdef(const char* __VA_ARGS__) body; \
			funcdef(const wchar_t* __VA_ARGS__) body; \
			funcdef(const char16_t* __VA_ARGS__) body; \
			funcdef(const char32_t* __VA_ARGS__) body; \


#endif