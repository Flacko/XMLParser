
#include "xmlparser.h"

const char* UTF16LE::name()
{
	return "UTF-16LE";
}

bool UTF16LE::letter (const char* c)
{
	return (ASCII::letter (c) and c[1] == '\0');
}

bool UTF16LE::digit (const char* c)
{
	return (ASCII::digit (c) and c[1] == '\0');
}

bool UTF16LE::hexdigit (const char* c)
{
	return (ASCII::hexdigit (c) and c[1] == '\0');
}

int UTF16LE::toint (const char* n, size_t l)
{
	if (l % 2) return 0;
	int sign = (n[0] == '-') ? 2 : 0;
	int r = 0;
	unsigned int e = 1;
	for (int i = l - 2; i >= sign; i -= 2)
	{
		if (!digit (n + i)) return 0;
		r += (n[i] - '0') * e;
		e = e * 10;
	}
	return (sign) ? -r : r;
}

unsigned int UTF16LE::touint (const char* n, size_t l)
{
	if (l % 2) return 0;
	unsigned int r = 0;
	unsigned int e = 1;
	for (int i = l - 2; i >= 0; i -= 2)
	{
		if (!digit (n + i)) return 0;
		r += (n[i] - '0') * e;
		e = e * 10;
	}
	return r;
}

int UTF16LE::xtoint (const char* n, size_t l)
{
	if (l % 2) return 0;
	int sign = (n[0] == '-') ? 2 : 0;
	int r = 0;
	unsigned int e = 1;
	for (int i = l - 2; i >= sign; i -= 2)
	{
		if (!hexdigit (n + i)) return 0;
		int c = n[i] - '0';
		if (n[i] >= 'a' and n[i] <= 'f')
		{
			c = n[i] - 'a' + 10;
		}
		else if (n[i] >= 'A' and n[i] <= 'F')
		{
			c = n[i] - 'A' + 10;
		}
		r += c * e;
		e = e * 16;
	}
	return (sign) ? -r : r;
}

unsigned int UTF16LE::xtouint (const char* n, size_t l)
{
	if (l % 2) return 0;
	unsigned int r = 0;
	unsigned int e = 1;
	for (int i = l - 2; i >= 0; i -= 2)
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

size_t UTF16LE::fget (FILE* f, char* dest, size_t max)
{
	if (max < 2) return 0;
	char buff[4];
	for (int i = 0; i < 2; i++) buff[i] = fgetc (f);
	if (buff[1] >> 2 == 0x36) //d8~db>>2 = 0x36
	{
		if (max < 4) return 0;
		for (int i = 2; i < 4; i++) buff[i] = fgetc (f);
		if (buff[3] >> 2 != 0x37) return 0; //dc~df>>2 = 0x37
		if (dest) for (int i = 0; i < 4; i++) dest[i] = buff[i];
		return 4;
	}
	if (dest) for (int i = 0; i < 2; i++) dest[i] = buff[i];
	return 2;
}

size_t UTF16LE::sget (const char* s, char* dest, size_t max)
{
	if (max < 2) return 0;
	if (s[1] >> 2 == 0x36)
	{
		if (max < 4) return 0;
		if (s[3] >> 2 != 0x37) return 0;
		if (dest) for (int i = 0; i < 4; i++) dest[i] = s[i];
		return 4;
	}
	if (dest) for (int i = 0; i < 2; i++) dest[i] = s[i];
	return 2;
}

size_t UTF16LE::encode (size_t p, char* dest, size_t max)
{
	if (max < 2) return 0;
	if (p <= 0xFFFF)
	{
		dest[0] = p % 0x100;
		dest[1] = (p - dest[0]) / 0x100;
		return 2;
	}
	if (max < 4) return 0;
	p = p - 0x10000;
	unsigned short lead = 0xD800 + (p >> 10); //first 10 bits
	unsigned short trail = 0xDC00 + (p & 0x3FF); //last 10 bits
	dest[0] = lead % 0x100;
	dest[1] = (lead - dest[0]) / 0x100;
	dest[2] = trail % 0x100;
	dest[3] = (trail - dest[2]) / 0x100;
	return 4;
}

size_t UTF16LE::decode (const char* s)
{
	const unsigned char* x = (const unsigned char*) s;
	if ( (x[1] >> 2) == 0x36)
	{
		if ( (x[3] >> 2) != 0x37) return 0;
		unsigned short lead = x[0] + x[1] * 0x100;
		unsigned short trail = x[2] + x[3] * 0x100;
		return 0x10000 + ( (lead - 0xD800) << 10) + ( (trail - 0xDC00));
	}
	return x[0] + x[1] * 0x100;
}
