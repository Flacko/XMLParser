
#ifndef _XMLDOCUMENT_H_
#define _XMLDOCUMENT_H_

class DLL_EXPORT Document
{
	public:
		Document();
		~Document();

		void readDocument (const utf8_t* s);
		void readDocument (const char* s);
		void parseFile (FILE* f, Node* parent, size_t line);
		Node* getBaseNode();
		Node* getDeclaration();
	private:
		Node* _baseNode;
		Node* _declaration;
		size_t readEntity (FILE* f, std::string& dest);
		size_t unescapeChar (const char* s, std::string& dest);

		bool isIdentifier (char c);

		void readIdentifier (FILE* f, char& c, size_t& l, std::string& dest);
		void readAttribute (FILE* f, char& c, size_t& l, std::string& name, std::string& val);

		void skipUntil (FILE* f, char& c, size_t& l, const char d);
		void skipWS (FILE* f, char& c, size_t& l);
};

#endif // XMLDOCUMENT_H
