
#ifndef _UTF8_H_
#define _UTF8_H_

class DLL_EXPORT UTF8
{
	public:
		static const char* name();

		static bool letter (const char* c) { return ASCII::letter (c); }
		static bool digit (const char* c) { return ASCII::digit (c); }
		static bool hexdigit (const char* c) { return ASCII::hexdigit (c); }

		static int toint (const char* n, size_t l) { return ASCII::toint (n, l); }
		static unsigned int touint (const char* n, size_t l) { return ASCII::touint (n, l); }
		static int xtoint (const char* n, size_t l) { return ASCII::xtoint (n, l); };
		static unsigned int xtouint (const char* n, size_t l) { return ASCII::xtouint (n, l); };

		static size_t fget (FILE* f, char* dest, size_t max);
		static size_t sget (const char* s, char* dest, size_t max);

		static size_t encode (size_t p, char* dest, size_t max);
		static size_t encode (size_t p, std::string& dest);
		static size_t decode (const char* s);

		static const size_t maxbytes = 6;

	private:
		static size_t headingbyte (const char c);

};
#endif // UTF8_H
