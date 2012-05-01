
#ifndef _UTF8_H_
#define _UTF8_H_

typedef unsigned char utf8_t;

class DLL_EXPORT UTF8
{
	public:
		static size_t fget (FILE* f, utf8_t* dest, size_t max);
		static size_t fget (FILE* f, char* dest, size_t max);

		static size_t sget (const utf8_t* s, utf8_t* dest, size_t max);
		static size_t sget (const utf8_t* s, char* dest, size_t max);
		static size_t sget (const char* s, utf8_t* dest, size_t max);
		static size_t sget (const char* s, char* dest, size_t max);

		static size_t strlen (const utf8_t* s);
		static size_t strlen (const char*);

		static size_t encode (size_t p, utf8_t* d, size_t max);
		static size_t encode (size_t p, char* d, size_t max);

		static bool validbyte (const utf8_t c);
		static size_t headingbyte (const utf8_t c);

	private:
};
#endif // UTF8_H
