
#include "xmlparser.h"

XML::Document::Document() : _baseNode (NULL), _declaration (NULL)
{
}
XML::Document::~Document()
{
	if (_baseNode) delete _baseNode;
	if (_declaration) delete _declaration;
}

void XML::Document::readDocument (const char* s)
{
	FILE* f = fopen ( (const char*) s, "rb");
	if (!f) throw fileNotFoundErr ( (const char*) s);

	unsigned char utf16bom[2];
	for (int i = 0; i < 2; i++) utf16bom[i] = fgetc (f);

	char c[10];
	if (utf16bom[0] == 0xFF and utf16bom[1] == 0xFE)
	{
		UTF16LE::fget (f, c, 10);
		parseFile <UTF16LE> (f, NULL, c, 1);
	}
	else if (utf16bom[0] == 0xFE and utf16bom[1] == 0xFF)
	{
		UTF16BE::fget(f,c,10);
		parseFile <UTF16BE> (f, NULL, c, 1);
	}
	else
	{
		rewind (f);
		ASCII::fget (f, c, 10);
		parseFile <ASCII> (f, NULL, c, 1);
	}
	fclose (f);
}

XML::Node* XML::Document::getDeclaration() { return _declaration; }
XML::Node* XML::Document::getBaseNode() { return _baseNode; }


template <class ENC>
void XML::Document::skipWS (FILE* f, char* c, size_t& l)
{
	size_t point = ENC::decode (c);
	while ( (point == ' ' or point == '\t' or point == '\n' or point == '\r'))
	{
		if (point == '\n') l++;
		ENC::fget (f, c, 10);
		point = ENC::decode (c);
	}
}

template <class ENC>
void XML::Document::skipUntil (FILE* f, char* c, size_t& l, const char d)
{
	size_t point = ENC::decode (c);
	while (point != (unsigned char) d)
	{
		if (point == '\n') l++;
		ENC::fget (f, c, 10);
		point = ENC::decode (c);
	}
}

template <class ENC>
bool XML::Document::isIdentifier (const char* c)
{
	return ENC::letter (c) or ENC::decode (c) == '_';
}

template <class ENC>
void XML::Document::readIdentifier (FILE* f, char* c, size_t& l, std::string& dest)
{
	if (isIdentifier <ENC> (c))
	{
		UTF8::encode (ENC::decode (c), dest);
		ENC::fget (f, c, 10);
	}
	while (isIdentifier <ENC> (c) or ENC::digit (c) or ENC::decode (c) == '-')
	{
		if (feof (f)) throw unexpectedErr (l, "EOF");
		UTF8::encode (ENC::decode (c), dest);
		ENC::fget (f, c, 10);
	}
}

template <class ENC>
void XML::Document::readAtt (FILE* f, char* c, size_t& l, std::string& attname, std::string& attval)
{
	readIdentifier <ENC> (f, c, l, attname);
	skipWS <ENC> (f, c, l);
	if (ENC::decode (c) != '=') throw unexpectedErr (l, c);
	ENC::fget (f, c, 10);
	skipWS <ENC> (f, c, l);
	if (ENC::decode (c) != '\"') throw unexpectedErr (l, c);
	ENC::fget (f, c, 10);
	size_t decode = ENC::decode (c);
	while (decode != '\"')
	{
		if (feof (f)) throw unexpectedErr (l, "EOF");
		if (decode == '&')
		{
			size_t off = readEntity <ENC> (f, attval);
			if (off == 0) throw expectedErr (l, "closing tag");
		}
		else
		{
			UTF8::encode (decode, attval);
		}
		ENC::fget (f, c, 10);
		decode = ENC::decode (c);
	}
}

template <class ENC>
void XML::Document::parseFile (FILE* f, XML::Node* parent, char* c, size_t line)
{
	skipWS <ENC> (f, c, line);
	size_t decode = ENC::decode (c);
	//the eof is not raised until tried to read beyond eof
	//so we move this return statement here
	if (feof (f)) return;
	if (decode == '<')
	{
		ENC::fget (f, c, 10);
		decode = ENC::decode (c);
		if (decode == '?') //info tag
		{
			_declaration = new XML::Node;
			ENC::fget (f, c, 10);
			readIdentifier <ENC> (f, c, line, _declaration->Tag);
			//declaration nodes can only tag as 'xml'
			if (_declaration->Tag.compare ("xml")) throw expectedErr (line, "xml");
			skipWS <ENC> (f, c, line);
			while (isIdentifier <ENC> (c))
			{
				std::string attname;
				std::string attval;
				readAtt <ENC> (f, c, line, attname, attval);
				_declaration->setAtt (attname, attval);
				ENC::fget (f, c, 10);
				skipWS <ENC> (f, c, line);
			}
			decode = ENC::decode (c);
			if (decode != '?') throw unexpectedErr (line, c);
			ENC::fget (f, c, 10);
			decode = ENC::decode (c);
			if (decode != '>') throw unexpectedErr (line, c);
			ENC::fget (f, c, 10);
			if (_declaration->getAtt ("encoding"))
			{
				if (!strcmp (_declaration->getAtt ("encoding"), ASCII::name()))
					return parseFile<ASCII> (f, parent, c, line);
				else if (!strcmp (_declaration->getAtt ("encoding"), UTF8::name()))
					return parseFile<UTF8> (f, parent, c, line);
				else if (!strcmp (_declaration->getAtt ("encoding"), UTF16LE::name()))
					return parseFile<UTF16LE> (f, parent, c, line);
				else if (!strcmp(_declaration->getAtt("encoding"), UTF16BE::name() ))
					return parseFile <UTF16BE> (f, parent, c, line);
				else
					throw encodingNotSupportedErr (_declaration->getAtt ("encoding"));
			}
			else
			{
				return parseFile <ENC> (f, parent, c, line);
			}
		}
		else if (decode == '!') //comment tag
		{
			for (int i = 0; i < 2; i++)
			{
				ENC::fget (f, c, 10);
				decode = ENC::decode (c);
				if (decode != '-') throw unexpectedErr (line, c);
			}
			char buff[4];
			buff[3] = '\0';
			while (strcmp (buff, "-->"))
			{
				ENC::fget (f, c, 10);
				decode = ENC::decode (c);
				if (decode == '\n') line++;
				if (feof (f)) throw expectedErr (line, "comment close");
				for (int i = 0; i < 3; i++) buff[i] = buff[i+1];
				buff[2] = decode;
			}
			ENC::fget (f, c, 10);
			return parseFile <ENC> (f, parent, c, line);
		}
		else if (decode == '/') //closing tag
		{
			ENC::fget (f, c, 10);
			if (!isIdentifier <ENC> (c)) throw unexpectedErr (line, c);
			std::string tagname;
			readIdentifier <ENC> (f, c, line, tagname);
			if (!parent) throw unexpectedErr (line, "closing tag");
			if (tagname.compare (parent->Tag)) throw expectedErr (line, parent->Tag.c_str());
			skipWS <ENC> (f, c, line);
			decode = ENC::decode (c);
			if (decode != '>') throw unexpectedErr (line, decode);
			ENC::fget (f, c, 10);
			return parseFile <ENC> (f, parent->getParent(), c, line);
		}
		else if (isIdentifier <ENC> (c)) // opening tag
		{
			XML::Node* node = new XML::Node;
			if (parent)
			{
				node->setParent (parent);
			}
			else if (!parent and !_baseNode)
			{
				_baseNode = node;
			}
			else if (!parent and _baseNode)
			{
				throw unexpectedErr (line, "extra root node");
			}
			readIdentifier <ENC> (f, c, line, node->Tag);
			skipWS <ENC> (f, c, line);

			while (isIdentifier <ENC> (c))
			{
				std::string attname;
				std::string attval;
				readAtt <ENC> (f, c, line, attname, attval);
				node->setAtt (attname, attval);
				ENC::fget (f, c, 10);
				skipWS <ENC> (f, c, line);
			}
			decode = ENC::decode (c);
			if (decode != '>') throw unexpectedErr (line, decode); //parsing error - '>' expected
			ENC::fget (f, c, 10);
			return parseFile <ENC> (f, node, c, line);
		}
	}
	else // inner content
	{
		if (!parent) { throw unexpectedErr (line, decode); }
		while (decode != '<')
		{
			if (feof (f)) throw expectedErr (line, "closing tag");
			if (decode == '&')
			{
				readEntity <ENC> (f, parent->Inner);
			}
			else
			{
				parent->Inner += decode;
			}
			ENC::fget (f, c, 10);
			decode = ENC::decode (c);
			if (decode == '\n')
			{
				skipWS <ENC> (f, c, line);
			}
			decode = ENC::decode (c);
		}
		//put back the < marker so the function can read it
		parseFile <ENC> (f, parent, c, line);
	}
}

template <class ENC>
size_t XML::Document::readEntity (FILE* f, std::string& dest)
{
	char buffer[10];
	memset (buffer, 0, 10);
	char c[10];
	ENC::fget (f, c, 10);
	for (int i = 0; i < 10; i++)
	{
		size_t decode = ENC::decode (c);
		if (decode == ';')
		{
			break;
		}
		else if (!ENC::letter (c) and !ENC::hexdigit (c) and decode != '#' and decode != 'x')
		{
			return 0; //parsing error
		}
		else if (decode > 0x7F)
		{
			return 0;
		}
		buffer[i] = decode;
		ENC::fget (f, c, 10);
	}
	return unescapeChar (buffer, dest);
}

size_t XML::Document::unescapeChar (const char* s, std::string& dest)
{
	const char* escaped[] = {"lt", "gt", "amp", "apos", "quot"};
	const char unescaped[] = {'<', '>', '&', '\'', '\"'};
	const size_t size = 5;

	for (size_t x = 0; x < size; x++)
	{
		if (!strcmp (s, escaped[x]))
		{
			dest[0] = unescaped[x];
			return 1;
		}
	}
	size_t slen = strlen (s);
	if (slen == 0) return 0;

	if (s[0] == '#')
	{
		if (slen == 1) return 0;
		char buff[10];
		memset (buff, 0, 10);

		if (s[1] == 'x')
		{
			if (slen == 2) return 0;
			size_t i = UTF8::encode (ASCII::xtouint (s + 2, slen - 2), buff, 9);
			dest += buff;
			return i;
		}
		else if (ASCII::digit (s + 1))
		{
			size_t i = UTF8::encode (ASCII::touint (s + 1, slen - 1), buff, 9);
			dest += buff;
			return i;
		}
	}
	return 0; // parsing error
}
