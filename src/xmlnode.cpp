
#include "xmlparser.h"


//Element
XML::Element::Element() : _parent(NULL), _tag(NULL)
{
}
XML::Element::~Element()
{
	delete _tag;
}

XML::Element* XML::Element::getParent() const
{
	return _parent;
}

const char* XML::Element::getTag() const
{
	return _tag;
}

void XML::Element::setTag(const std::string& t)
{
	char* buff = new char[t.length()+1];
	strcpy(buff,t.c_str());
	_tag = buff;
}

void XML::Element::setTag(const char* t)
{
	char* buff = new char[strlen(t)+1];
	strcpy(buff,t);
	_tag = buff;
}

const char* XML::Element::getText() const { return NULL; }
void XML::Element::setText(const std::string&) { }
void XML::Element::setText(const char*) { }

//Node
XML::Node::~Node()
{
	for (childlist_t::iterator it = _children.begin(); it != _children.end(); it++)
	{
		delete (*it);
	}
}

XML::childlist_t* XML::Node::getChildren()
{
	return &_children;
}

void XML::Node::appendChild(Element* c)
{
	if(!c->getParent())
	{
		c->_parent = this;
		childlist_t::iterator it = _children.end();
		_children.insert(it,c);
	}
}

XML::attmap_t* XML::Node::getAttMap()
{
	return &_att;
}

const char* XML::Node::getAtt(const std::string& n)
{
	attmap_t::iterator it = _att.find(n);
	if(it == _att.end()) return NULL;
	return (*it).second.c_str();
}
const char* XML::Node::getAtt(const char* n)
{
	return getAtt(std::string(n));
}

const char* XML::Node::operator [] (const std::string& n)
{
	return getAtt(n);
}

const char* XML::Node::operator [] (const char* n)
{
	return getAtt(n);
}

void XML::Node::setAtt (const std::string& n, const std::string& v)
{
	_att[n] = v;
}

void XML::Node::setAtt (const char* n, const char* v)
{
	setAtt(std::string(n), std::string(v));
}
