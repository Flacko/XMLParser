
#include "xmlparser.h"

bool ASCII::validbyte (const unsigned char c)
{
	if (c <= 0x7F) return true; //0xxxxxxx

	return false;
}

bool ASCII::letter (const unsigned char c)
{
	if (c >= 'a' and c <= 'z') return true;

	if (c >= 'A' and c <= 'Z') return true;

	return false;
}

bool ASCII::digit (const unsigned char c)
{
	return (c >= '0' and c <= '9');
}

bool ASCII::hexdigit (const unsigned char c)
{
	return ASCII::digit (c) or (c >= 'a' and c <= 'f') or (c >= 'A' and c <= 'F');
}

void ASCII::tolower (char* s)
{
	size_t l = strlen (s);

	for (size_t i = 0; i < l; i++)
		if (s[i] >= 'A' and s[i] <= 'Z')
			s[i] += ('a' - 'A');
}

void ASCII::toupper (char* s)
{
	size_t l = strlen (s);

	for (size_t i = 0; i < l; i++)
		if (s[i] >= 'a' and s[i] <= 'z')
			s[i] -= ('a' - 'A');
}

int ASCII::toint (const char* n)
{
	int slen = strlen (n);
	int r = 0;
	int e = 1;
	bool sign = (n[0] == '-');

	for (int i = slen - 1; i >= sign; i--)
	{
		if (n[i] < '0' or n[i] > '9') return 0;

		r = r + (n[i] - '0') * e;
		e = e * 10;
	}

	if (sign) return -r;

	return r;
}

unsigned int ASCII::touint (const char* n)
{
	int slen = strlen (n);
	unsigned int r = 0;
	int e = 1;

	for (int i = slen - 1; i >= 0; i--)
	{
		if (n[i] < '0' or n[i] > '9') return 0;

		r = r + (n[i] - '0') * e;
		e = e * 10;
	}

	return r;
}

int ASCII::xtoint (const char* n)
{
	int slen = strlen (n);
	int r = 0;
	int e = 1;
	bool sign = (n[0] == '-');

	for (int i = slen - 1; i >= sign; i--)
	{
		if (!hexdigit (n[i])) return 0;

		int c = n[i] - '0';

		if (n[i] >= 'a' and n[i] <= 'f') c = n[i] - 'a' + 10;

		if (n[i] >= 'A' and n[i] <= 'F') c = n[i] - 'A' + 10;

		r += c * e;
		e = e * 16;
	}

	if (sign) return -r;

	return r;
}

unsigned int ASCII::xtouint (const char* n)
{
	int slen = strlen (n);
	int r = 0;
	int e = 1;

	for (int i = slen - 1; i >= 0; i--)
	{
		if (!hexdigit (n[i])) return 0;

		int c = n[i] - '0';

		if (n[i] >= 'a' and n[i] <= 'f') c = n[i] - 'a' + 10;

		if (n[i] >= 'A' and n[i] <= 'F') c = n[i] - 'A' + 10;

		r += c * e;
		e = e * 16;
	}

	return r;
}
