
#include "xmlparser.h"

const char* ASCII::name()
{
	return "ASCII";
}

bool ASCII::letter (const char* c)
{
	if (c[0] >= 'a' and c[0] <= 'z') return true;
	if (c[0] >= 'A' and c[0] <= 'Z') return true;
	return false;
}

bool ASCII::digit (const char* c)
{
	return (c[0] >= '0' and c[0] <= '9');
}

bool ASCII::hexdigit (const char* c)
{
	return ASCII::digit (c) or (c[0] >= 'a' and c[0] <= 'f') or (c[0] >= 'A' and c[0] <= 'F');
}

int ASCII::toint (const char* n, size_t l)
{
	int r = 0;
	unsigned int e = 1;
	bool sign = (n[0] == '-');

	for (int i = l - 1; i >= sign; i--)
	{
		if (!digit (n + i)) return 0;
		r = r + (n[i] - '0') * e;
		e = e * 10;
	}
	return (sign) ? -r : r;
}

unsigned int ASCII::touint (const char* n, size_t l)
{
	unsigned int r = 0;
	unsigned int e = 1;

	for (int i = l - 1; i >= 0; i--)
	{
		if (n[i] < '0' or n[i] > '9') return 0;

		r = r + (n[i] - '0') * e;
		e = e * 10;
	}
	return r;
}

int ASCII::xtoint (const char* n, size_t l)
{
	int r = 0;
	unsigned int e = 1;
	bool sign = (n[0] == '-');

	for (int i = l - 1; i >= sign; i--)
	{
		if (!hexdigit (n + i)) return 0;
		int c = n[i] - '0';
		if (n[i] >= 'a' and n[i] <= 'f') c = n[i] - 'a' + 10;
		if (n[i] >= 'A' and n[i] <= 'F') c = n[i] - 'A' + 10;
		r += c * e;
		e = e * 16;
	}
	return (sign) ? -r : r;
}

unsigned int ASCII::xtouint (const char* n, size_t l)
{
	unsigned int r = 0;
	unsigned int e = 1;

	for (int i = l - 1; i >= 0; i--)
	{
		if (!hexdigit (n + i)) return 0;
		int c = n[i] - '0';
		if (n[i] >= 'a' and n[i] <= 'f') c = n[i] - 'a' + 10;
		if (n[i] >= 'A' and n[i] <= 'F') c = n[i] - 'A' + 10;
		r += c * e;
		e = e * 16;
	}
	return r;
}

size_t ASCII::fget (FILE* f, char* dest, size_t max)
{
	if (max < 1) return 0;
	dest[0] = fgetc (f);
	return 1;
}

size_t ASCII::sget (const char* s, char* dest, size_t max)
{
	if (max < 1) return 0;
	dest[0] = s[0];
	return 1;
}

/*size_t ASCII::strlen(const char* s)
{
	return strlen(s);
}*/

size_t ASCII::encode (size_t p, char* dest, size_t max)
{
	if (p > 0x7F) return 0;
	if (max < 1) return 0;
	dest[0] = p;
	return 1;
}

size_t ASCII::decode (const char* s)
{
	return s[0];
}
