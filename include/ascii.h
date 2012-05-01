

#ifndef _ASCII_H_
#define _ASCII_H_

class DLL_EXPORT ASCII
{
	public:
		static bool validbyte (const unsigned char c);
		static bool letter (const unsigned char c);
		static bool digit (const unsigned char c);
		static bool hexdigit (const unsigned char c);
		static void tolower (char* s);
		static void toupper (char* s);
		static int toint (const char* n);
		static unsigned int touint (const char* n);
		static int xtoint (const char* n);
		static unsigned int xtouint (const char* n);
};

#endif // ASCII_H_
