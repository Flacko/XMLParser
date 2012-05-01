
#include "xmlparser.h"

XML::Document::Document() : _baseNode (NULL), _declaration (NULL)
{
}
XML::Document::~Document()
{
	if (_baseNode) delete _baseNode;
	if (_declaration) delete _declaration;
}

void XML::Document::readDocument (const utf8_t* s)
{
	FILE* f = fopen ( (const char*) s, "rb");
	parseFile (f, NULL, 1);
	fclose (f);
}
void XML::Document::readDocument (const char* s)
{
	readDocument ( (utf8_t*) s);
}

XML::Node* XML::Document::getDeclaration() { return _declaration; }
XML::Node* XML::Document::getBaseNode() { return _baseNode; }

void XML::Document::skipWS (FILE* f, char& c, size_t& l)
{
	while ( (c == ' ' or c == '\t' or c == '\n' or c == '\r'))
	{
		if (c == '\n') l++;

		c = fgetc (f);
	}
}

void XML::Document::skipUntil (FILE* f, char& c, size_t& l, const char d)
{
	while (c != d) \
	{
		if (c == '\n') l++;

		c = fgetc (f);
	}
}

bool XML::Document::isIdentifier (const char c)
{
	return ( (UTF8::validbyte (c) and !ASCII::validbyte (c)) or ASCII::letter (c) or c == '_');
}

void XML::Document::readIdentifier (FILE* f, char& c, size_t& l, std::string& dest)
{
	if (isIdentifier (c))
	{
		dest += c;
		c = fgetc (f);
	}
	while(isIdentifier (c) or ASCII::digit (c) or c == '-')
	{
		if(feof(f)) throw unexpectedErr (l, "EOF");
		dest += c;
		c = fgetc (f);
	}
}

void XML::Document::readAttribute (FILE* f, char& c, size_t& l, std::string& attname, std::string& attval)
{
	readIdentifier (f, c, l, attname);
	skipWS (f, c, l);
	if (c != '=') throw unexpectedErr (l, c);
	c = fgetc (f);
	skipWS (f, c, l);
	if (c != '\"') throw unexpectedErr (l, c);
	c = fgetc (f);
	while(c != '\"')
	{
		if(feof(f)) throw unexpectedErr (l, "EOF");
		if (c == '&')
		{
			size_t off = readEntity (f, attval);
			if (off == 0) throw expectedErr (l, "closing tag");
		}
		else
		{
			attval += c;
		}
		c = fgetc (f);
	}
}

void XML::Document::parseFile (FILE* f, XML::Node* parent, size_t line)
{
	char c = fgetc(f);
	skipWS (f, c, line);
	//the eof is not raised until tried to read beyond eof
	//so we move this return statement here
	if(feof(f)) return;

	if (c == '<')
	{
		c = fgetc (f);
		if (c == '?') //info tag
		{
			_declaration = new XML::Node;
			c = fgetc (f);
			readIdentifier (f, c, line, _declaration->Tag);
			//declaration nodes can only tag as 'xml'
			if (_declaration->Tag.compare("xml")) throw expectedErr (line, "xml");
			skipWS (f, c, line);
			while (isIdentifier (c))
			{
				std::string attname;
				std::string attval;
				readAttribute (f, c, line, attname, attval);
				_declaration->setAtt (attname, attval);
				c = fgetc (f);
				skipWS (f, c, line);
			}

			if (c != '?') throw unexpectedErr (line, c);
			c = fgetc (f);
			if (c != '>') throw unexpectedErr (line, c);
			return parseFile (f, parent, line);
		}
		else if (c == '!') //comment tag
		{
			for (int i = 0; i < 2; i++)
			{
				c = fgetc (f);
				if (c != '-') throw unexpectedErr (line, c);
			}
			char buff[4];
			buff[3] = '\0';
			while (strcmp (buff, "-->"))
			{
				c = fgetc (f);
				if (c == '\n') line++;
				if (feof (f)) throw expectedErr (line, "comment close");
				for (int i = 0; i < 3; i++) buff[i] = buff[i+1];
				buff[2] = c;
			}

			return parseFile (f, parent, line);
		}
		else if (c == '/') //closing tag
		{
			c = fgetc (f);
			if (!isIdentifier (c)) throw unexpectedErr (line, c);
			std::string tagname;
			readIdentifier (f, c, line, tagname);
			if (!parent) throw unexpectedErr (line, "closing tag");
			if (tagname.compare(parent->Tag)) throw expectedErr (line, parent->Tag.c_str());
			skipWS (f, c, line);
			if (c != '>') throw unexpectedErr (line, c);
			return parseFile (f, parent->getParent(), line);
		}
		else if (isIdentifier (c)) // opening tag
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
			readIdentifier (f, c, line, node->Tag);
			skipWS (f, c, line);

			while (isIdentifier (c))
			{
				std::string attname;
				std::string attval;
				readAttribute (f, c, line, attname, attval);
				node->setAtt (attname, attval);
				c = fgetc (f);
				skipWS (f, c, line);
			}
			if (c != '>') throw unexpectedErr (line, c); //parsing error - '>' expected
			return parseFile (f, node, line);
		}
	}
	else // inner content
	{
		if(!parent) { throw unexpectedErr (line, c); }
		while(c != '<')
		{
			if(feof(f)) throw expectedErr(line,"closing tag");
			if(c == '&')
			{
				readEntity(f, parent->Inner);
			}
			else
			{
				parent->Inner += c;
			}
			c = fgetc(f);
			if(c == '\n')
			{
				skipWS(f,c,line);
			}
		}
		//put back the < marker so the function can read it
		ungetc('<',f);
		parseFile(f, parent, line);
	}
}

size_t XML::Document::readEntity (FILE* f, std::string& dest)
{
	char buffer[10];
	memset(buffer, 0, 10);
	char c = fgetc (f);

	for (int i = 0; i < 10; i++)
	{
		if (c == ';') break;
		else if (!ASCII::letter (c) and !ASCII::hexdigit (c) and c != '#' and c != 'x')
		{
			return 0; //parsing error
		}

		buffer[i] = c;
		c = fgetc (f);
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
		memset(buff,0,10);

		if (s[1] == 'x')
		{
			if (slen == 2) return 0;
			const char* ptr = &s[2];
			size_t i = UTF8::encode (ASCII::xtouint (ptr), buff, 9);
			dest += buff;
			return i;
		}
		else if (ASCII::digit (s[1]))
		{
			const char* ptr = &s[1];
			size_t i = UTF8::encode (ASCII::touint (ptr), buff, 9);
			dest += buff;
			return i;
		}
	}
	return 0; // parsing error
}
