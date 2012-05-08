
#ifndef _XMLDOCUMENT_H_
#define _XMLDOCUMENT_H_

class DLL_EXPORT Document
{
	public:
		Document();
		~Document();

		void readDocument (const char* s);
		Node* getBaseNode();
		Node* getDeclaration();
	private:
		Node* _baseNode;
		Node* _declaration;

		template <class ENC> size_t readEntity (FILE* f, std::string& dest);
		size_t unescapeChar (const char* s, std::string& dest);

		template <class ENC> bool isIdentifier (const char* c);
		template <class ENC> void readIdentifier (FILE* f, char* c, size_t& l, std::string& dest);
		template <class ENC> void readAtt (FILE* f, char* c, size_t& l, std::string& name, std::string& val);

		template <class ENC> void skipUntil (FILE* f, char* c, size_t& l, const char d);
		template <class ENC> void skipWS (FILE* f, char* c, size_t& l);

		template <class ENC> void parseFile (FILE* f, Node* parent, char* c, size_t line);
};

#endif // XMLDOCUMENT_H
