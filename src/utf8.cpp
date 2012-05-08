
#include "xmlparser.h"

const char* UTF8::name()
{
	return "UTF-8";
}

size_t UTF8::fget (FILE* f, char* dest, size_t max)
{
	char buff[6];
	buff[0] = fgetc (f);
	size_t size = headingbyte (buff[0]);

	if (size > max) { return 0; }
	if (size == 0) { return 0; }

	for (size_t i = 1; i < size; i++)
	{
		buff[i] = fgetc (f);
		if (buff[i] >> 6 != 2) return 0; //10xxxxxx
	}
	if (dest) for (size_t i = 0; i < size; i++) { dest[i] = buff[i]; }
	return size;
}

size_t UTF8::sget (const char* s, char* dest, size_t max)
{
	const unsigned char first = s[0];
	size_t size = UTF8::headingbyte (first);

	if (size > max) return 0;
	if (size == 0) return 0;
	if (dest) dest[0] = first;

	for (size_t i = 1; i < size; i++)
	{
		unsigned char t = s[i];
		if (t > 0xBF or t < 0x80)
		{
			if (dest) memset (dest, 0, i);
			return 0;
		}
		if (dest) dest[i] = t;
	}
	return size;
}

/*size_t UTF8::strlen (const char* s)
{
	size_t len = 0;
	for (size_t i = 0; s[i] != '\0'; len++)
	{
		int l = sget (s + i, NULL, 6);
		if (!l) return 0;
		i += l;
	}
	return len;
}*/

size_t UTF8::encode (size_t p, char* d, size_t max)
{
	const size_t bytes[] = {0x7F, 0x07FF, 0xFFFF, 0x1FFFFF, 0x3FFFFF, 0x07FFFFFF};
	size_t sz = 0;

	for (sz = 1; sz < sizeof (bytes) / sizeof (size_t); sz++)
	{
		if (bytes[sz] >= p) break;
	}
	if (sz == 0) return 0;
	if (sz > max) return 0;

	const int binary[] = {0x00, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC};
	const int bits[] = {0x7F, 0x1F, 0xF, 0x08, 0x02, 0x01};

	d[0] = binary[sz-1] + ( (p >> (sz - 1) * 6) & bits[sz-1]);

	for (size_t i = 1; i < sz; i++)
	{
		d[i] = 0x80 + ( (p >> (sz - 1 - i) * 6) & 0x3F);
	}

	return sz;
}

size_t UTF8::encode (size_t p, std::string& dest)
{
	char buff[maxbytes+1];
	size_t s = encode (p, buff, maxbytes + 1);
	buff[s] = '\0';
	dest += buff;
	return s;
}

size_t UTF8::decode (const char* s)
{
	const int bits[] = {0x7F, 0x1F, 0xF, 0x08, 0x02, 0x01};
	size_t l = headingbyte (s[0]);
	size_t r = s[0] & bits[l-1];
	for (size_t i = 1; i < l; i++)
	{
		r = (r << 6) + (s[i] & 0x3F);
	}
	return r;
}

size_t UTF8::headingbyte (const char c)
{
	unsigned char b = (unsigned char) c;
	if ( (b & 128) == 0) return 1; // 0xxxxxxx
	int e = 32; //xx1xxxxx
	for (int x = 2; x <= 6; x++)
	{
		if (! (b & e)) return x;
		e = e / 2;
	}
	return 0;
}
