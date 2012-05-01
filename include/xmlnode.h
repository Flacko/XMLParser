
#ifndef _XMLNODE_H_
#define _XMLNODE_H_

class Node;

typedef std::map<std::string, std::string, mapStringComp> attmap_t;
typedef std::list<Node*> childlist_t;

class DLL_EXPORT Node
{
	public:
		Node();
		~Node();

		Node* getParent() const;
		void setParent (Node* p);

		std::string Tag;
		std::string Inner;

		childlist_t* getChildren();

		void setAtt (const char* n, const char* v);
		void setAtt (const std::string& n, const std::string& v);

		attmap_t* getAttMap();
		const char* getAtt (const char* n);
		const char* getAtt (const std::string& n);

		const char* operator [] (const char* n);
		const char* operator [] (const std::string& n);

	private:
		Node* _parent;
		attmap_t _att;
		childlist_t _children;
};

#endif // XMLNODE_H
