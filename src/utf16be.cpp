
#include "xmlparser.h"

const char* UTF16BE::name()
{
	return "UTF-16BE";
}

bool UTF16BE::letter (const char* c)
{
	return (ASCII::letter (c + 1) and c[0] == '\0');
}

bool UTF16BE::digit (const char* c)
{
	return (ASCII::digit (c + 1) and c[0] == '\0');
}

bool UTF16BE::hexdigit (const char* c)
{
	return (ASCII::hexdigit (c + 1) and c[0] == '\0');
}

int UTF16BE::toint (const char* n, size_t l)
{
	if (l % 2) return 0;
	int sign = (n[1] == '-') ? 2 : 0;
	int r = 0;
	unsigned int e = 1;
	for (int i = l - 2; i >= sign; i -= 2)
	{
		if (!digit (n + i)) return 0;
		r += (n[i+1] - '0') * e;
		e = e * 10;
	}
	return (sign) ? r : -r;
}

size_t UTF16BE::touint (const char* n, size_t l)
{
	if (l % 2) return 0;
	size_t r = 0;
	unsigned int e = 1;
	for (int i = l - 2; i >= 0; i -= 2)
	{
		if (!digit (n + i)) return 0;
		r += (n[i+1] - '0') * e;
		e = e * 10;
	}
	return r;
}

int UTF16BE::xtoint (const char* n, size_t l)
{
	if (l % 2) return 0;
	int sign = (n[1] == '-') ? 2 : 0;
	int r = 0;
	unsigned int e = 1;
	for (int i = l - 2; i >= sign; i -= 2)
	{
		if (!hexdigit (n+i)) return 0;
		int c = n[i+1] - '0';
		if (n[i+1] >= 'a' and n[i+1] <= 'f')
		{
			c = n[i+1] - 'a' + 10;
		}
		else if (n[i+1] >= 'A' and n[i+1] <= 'F')
		{
			c = n[i+1] - 'A' + 10;
		}
		r += c * e;
		e = e * 16;
	}
	return (sign) ? -r : r;
}

size_t UTF16BE::xtouint(const char* n, size_t l)
{
	if(l%2) return 0;
	size_t r = 0;
	unsigned int e = 1;
	for(int i=l-2; i>=0; i-=2)
	{
		if(!hexdigit(n+i)) return 0;
		int c = n[i+1] - '0';
		if(n[i+1] >= 'a' and n[i+1] <='f')
		{
			c = n[i+1] - 'a' + 10;
		}
		else if (n[i+1] >= 'A' and n[i+1] <= 'F')
		{
			c = n[i+1] - 'A' + 10;
		}
		r += c*e;
		e = e * 16;
	}
	return r;
}

size_t UTF16BE::fget(FILE* f, char* dest, size_t max)
{
	if (max < 2) return 0;
	char buff[4];
	for (int i = 0; i < 2; i++) buff[i] = fgetc (f);
	if (buff[0] >> 2 == 0x36) //d8~db>>2 = 0x36
	{
		if (max < 4) return 0;
		for (int i = 2; i < 4; i++) buff[i] = fgetc (f);
		if (buff[2] >> 2 != 0x37) return 0; //dc~df>>2 = 0x37
		if (dest) for (int i = 0; i < 4; i++) dest[i] = buff[i];
		return 4;
	}
	if (dest) for (int i = 0; i < 2; i++) dest[i] = buff[i];
	return 2;
}

size_t UTF16BE::sget(const char* s, char* dest, size_t max)
{
	if (max < 2) return 0;
	if (s[0] >> 2 == 0x36)
	{
		if (max < 4) return 0;
		if (s[2] >> 2 != 0x37) return 0;
		if (dest) for (int i = 0; i < 4; i++) dest[i] = s[i];
		return 4;
	}
	if (dest) for (int i = 0; i < 2; i++) dest[i] = s[i];
	return 2;
}

size_t UTF16BE::encode(size_t p, char* dest, size_t max)
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
	dest[0] = (lead - dest[0]) / 0x100;
	dest[1] = lead % 0x100;
	dest[2] = (trail - dest[2]) / 0x100;
	dest[3] = trail % 0x100;
	return 4;
}

size_t UTF16BE::decode (const char* s)
{
	const unsigned char* x = (const unsigned char*) s;
	if ( (x[0] >> 2) == 0x36)
	{
		if ( (x[2] >> 2) != 0x37) return 0;
		unsigned short lead = x[0] * 0x100 + x[1];
		unsigned short trail = x[2] * 0x100 + x[3];
		return 0x10000 + ( (lead - 0xD800) << 10) + ( (trail - 0xDC00));
	}
	return x[0] * 0x100 + x[1];
}
