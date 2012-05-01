
#include <cstdio>
#include <map>
#include <list>
#include <string>
#include <exception>
#include <windows.h>

#ifdef BUILD_DLL
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT __declspec(dllimport)
#endif

#include "ascii.h"
#include "utf8.h"
namespace XML
{
	struct mapStringComp
	{
		bool operator() (const std::string& a, const std::string& b)
		{
			return a.compare(b)<0;
		}
	};
	class DLL_EXPORT Error : public std::exception
	{
		public:
			char msg[128];
			virtual const char* what()
			{
				return msg;
			}
	};
	class DLL_EXPORT expectedErr : public Error
	{
		public:
			expectedErr (size_t l, const char* s)
			{
				sprintf (msg, "Line:%u:Expected %s", l, s);
			}
			expectedErr (size_t l, const char c)
			{
				sprintf (msg, "Line:%u:Expected character '%c' (0x%x)", l, c, c);
			}
	};
	class DLL_EXPORT unexpectedErr : public Error
	{
		public:
			unexpectedErr (size_t l, const char* s)
			{
				sprintf (msg, "Line:%u:Unexpected %s", l, s);
			}
			unexpectedErr (size_t l, const char c)
			{
				sprintf(msg, "Line:%u:Unexpected character: '%c' (0x%x)", l, c, c);
			}
	};
#include "xmlnode.h"
#include "xmldocument.h"
}
