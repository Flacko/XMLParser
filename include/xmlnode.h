
#ifndef _XMLELEMENT_H_
#define _XMLELEMENT_H_

class Element;
class Text;
class Node;

class DLL_EXPORT Element
{
	friend class Node;
	public:
		Element();
		virtual ~Element();
		Element* getParent() const;

		virtual const char* getTag() const;
		virtual void setTag(const std::string& t);
		virtual void setTag(const char* t);

		virtual const char* getText() const;
		virtual void setText(const std::string& t);
		virtual void setText(const char* t);

	protected:
		Element* _parent;
		const char* _tag;
};

class DLL_EXPORT Text : public Element
{
	public:
		virtual const char* getTag() const { return NULL; };
		virtual void setTag(const std::string&) { };
		virtual void setTag(const char*) { };

		virtual const char* getText() const { return Element::getTag(); };
		virtual void setText(const std::string& t) { Element::setTag(t); };
		virtual void setText(const char* t) { Element::setTag(t); };
};

typedef std::map<std::string, std::string, mapStringComp> attmap_t;
typedef std::list<Element*> childlist_t;

class DLL_EXPORT Node : public Element
{
	public:
		~Node();
		childlist_t* getChildren();
		void appendChild(Element* c);

		attmap_t* getAttMap();

		const char* getAtt (const std::string& n);
		const char* getAtt (const char* n);

		const char* operator [] (const std::string& n);
		const char* operator [] (const char* n);

		void setAtt (const std::string& n, const std::string& v);
		void setAtt (const char* n, const char* v);

	private:
		attmap_t _att;
		childlist_t _children;
};

#endif
