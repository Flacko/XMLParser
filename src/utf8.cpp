
#include "xmlparser.h"

size_t UTF8::fget (FILE* f, utf8_t* dest, size_t max)
{
	const utf8_t first = fgetc (f);
	size_t size = UTF8::headingbyte (first);

	if (size > max) { return 0; }

	if (size == 0) { return 0; }

	if (dest) dest[0] = first;

	for (size_t i = 1; i < size; i++)
	{
		const utf8_t t = fgetc (f);

		if (t > 0xBF or t < 0x80)
		{
			fseek (f, -3, SEEK_CUR);

			if (dest) for (size_t j = i - 1; i <= i; i--) dest[j] = '\0';

			return 0;
		}

		if (dest) dest[i] = t;
	}

	return size;
}
size_t UTF8::fget (FILE* f, char* dest, size_t max) {return fget (f, (utf8_t*) dest, max);}

size_t UTF8::sget (const utf8_t* s, utf8_t* dest, size_t max)
{
	const utf8_t first = s[0];
	size_t size = UTF8::headingbyte (first);

	if (size > max) return 0;

	if (size == 0) return 0;

	if (dest) dest[0] = first;

	for (size_t i = 1; i < size; i++)
	{
		utf8_t t = s[i];

		if (t > 0xBF or t < 0x80)
		{
			if (dest) for (size_t j = i - 1; j <= i; j--) dest[j] = '\0';

			return 0;
		}

		if (dest) dest[i] = t;
	}

	return size;
}
size_t UTF8::sget (const utf8_t* s, char* dest, size_t max) { return sget (s, (utf8_t*) dest, max); }
size_t UTF8::sget (const char* s, utf8_t* dest, size_t max) { return sget ( (utf8_t*) s, dest, max); }
size_t UTF8::sget (const char* s, char* dest, size_t max) { return sget ( (utf8_t*) s, (utf8_t*) dest, max); }

size_t UTF8::strlen (const utf8_t* s)
{
	size_t len = 0;
	for(size_t i=0; s[i] != '\0'; len++)
	{
		int l = sget (s + i, (utf8_t*) NULL, 6);
		if (!l) return 0;
		i += l;
	}
	return len;
}
size_t UTF8::strlen (const char* s) {	return strlen ( (utf8_t*) s); }

size_t UTF8::encode (size_t p, utf8_t* d, size_t max)
{
	const size_t bytes[] = {0x7F, 0x07FF, 0xFFFF, 0x1FFFFF, 0x3FFFFF, 0x07FFFFFF};
	size_t sz = 0;

	for (sz = 0; sz < sizeof (bytes) / sizeof (size_t); sz++)
	{
		if (bytes[sz] >= p)
		{
			sz++;
			break;
		}
	}

	if (sz == 0) return 0;

	if (sz == max) return 0;

	const int binary[] = {0x00, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC};
	const int bits[] = {0x7F, 0x1F, 0xF, 0x08, 0x02, 0x01};


	utf8_t first = binary[sz-1] + ( (p >> (sz - 1) * 6) & bits[sz-1]);
	d[0] = first;

	for (size_t i = 1; i < sz; i++)
	{
		d[i] = 0x80 + ( (p >> (sz - 1 - i) * 6) & 0x3F);
	}

	return sz;
}
size_t UTF8::encode (size_t p, char* d, size_t max) { return encode (p, (utf8_t*) d, max); }

bool UTF8::validbyte (const utf8_t c)
{
	if (headingbyte (c)) return true;
	else if (c >> 6 == 2) return true; //10xxxxxx

	return false;
}

size_t UTF8::headingbyte (const utf8_t c)
{
	//0xxxxxxx, 110xxxxx, 1110xxxx, 11110xxx, 111110xx, 1111110x
	const int bytes[] = {0, 6, 14, 30, 62, 126};
	const int shifts[] = {7, 5, 4, 3, 2, 1};

	for (size_t i = 1; i < sizeof (bytes) / sizeof (int); i++)
	{
		if (c >> shifts[i-1] == bytes[i-1]) return i;
	}

	return 0;
}
