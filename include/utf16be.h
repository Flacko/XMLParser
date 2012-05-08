
#ifndef _UTF16BE_H_
#define _UTF16BE_H_

class DLL_EXPORT UTF16BE
{
	public:
		static const char* name();

		static bool letter (const char* c);
		static bool digit (const char* c);
		static bool hexdigit (const char* c);

		static int toint (const char* n, size_t l);
		static unsigned int touint (const char* n, size_t l);
		static int xtoint (const char* n, size_t l);
		static unsigned int xtouint (const char* n, size_t l);

		static size_t fget (FILE* f, char* dest, size_t max);
		static size_t sget (const char* s, char* dest, size_t max);

		static size_t encode (size_t p, char* dest, size_t max);
		static size_t decode (const char* s);

		static const size_t maxbytes = 4;
};

#endif // UTF16BE
