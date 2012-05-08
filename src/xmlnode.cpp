
#include "xmlparser.h"

XML::Node::Node() : _parent (NULL)
{
}

XML::Node::~Node()
{
	for (childlist_t::iterator it = _children.begin(); it != _children.end(); it++)
	{
		delete (*it);
	}
}

XML::Node* XML::Node::getParent() const
{
	return _parent;
}

void XML::Node::setParent (Node* p)
{
	_parent = p;
	std::list<XML::Node*>::iterator it = p->_children.end();
	p->_children.insert (it, this);
}

XML::childlist_t* XML::Node::getChildren()
{
	return &_children;
}

void XML::Node::setAtt (const char* n, const char* v)
{
	_att[std::string (n) ] = std::string (v);
}
void XML::Node::setAtt (const std::string& n, const std::string& v)
{
	_att[n] = v;
}

XML::attmap_t* XML::Node::getAttMap()
{
	return &_att;
}

const char* XML::Node::getAtt (const char* n)
{
	attmap_t::iterator it = _att.find (std::string (n));
	if (it == _att.end()) return NULL;
	return (*it).second.c_str();
}
const char* XML::Node::getAtt (const std::string& n)
{
	attmap_t::iterator it = _att.find (n);
	if (it == _att.end()) return NULL;
	return (*it).second.c_str();
}

const char* XML::Node::operator[] (const char* n)
{
	return getAtt (n);
}
const char* XML::Node::operator[] (const std::string& n)
{
	return getAtt (n);
}
