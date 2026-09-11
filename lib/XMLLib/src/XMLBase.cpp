// XMLBase.cpp: implementation of the XMLBase class.
//
//////////////////////////////////////////////////////////////////////



#include "XMLBase.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CXMLBase::CXMLBase(const QString &strRoot)
{
	//m_DomDoc = new QDomDocument(strRoot);
	m_DomDoc = new QDomDocument(QDomDocumentType());
	QDomElement root = m_DomDoc->createElement(strRoot);
	m_DomDoc->appendChild(root);
	m_DomDoc->toDocumentType();
	CurrentNode(root);
}

CXMLBase::CXMLBase(const QFile &file)
{
	m_DomDoc = new QDomDocument();
	m_DomDoc->setContent((QIODevice*)(&file));
}


CXMLBase::~CXMLBase()
{
	delete m_DomDoc;
}

QDomElement CXMLBase::Root() const
{
	return m_DomDoc->documentElement();
}

// finds the first element in the tree named 'element'
QDomElement CXMLBase::FindElement(const QString element) const
{
	QDomNodeList elements = m_DomDoc->elementsByTagName(element); // create a list of all elements named 'element'
	QDomElement node;

	for(int i = 0; i < (int)elements.count(); i++)
	{
		node = elements.item(i).toElement();
		if(node.nodeName() == element)
			return node;
	}

	return QDomElement();
}

QDomNode CXMLBase::AddElement(const QString &parent,const QString &element)
{
	QDomElement node = FindElement(parent);
	if(node.isNull())
		return node;
	
	QDomElement new_element = m_DomDoc->createElement(element);
	node.appendChild(new_element);
	return new_element;
	
}

QDomNode CXMLBase::AddElement(const QString &element)
{
	if(CurrentNode().isNull())
		return QDomNode();

	QDomElement new_element = m_DomDoc->createElement(element);
	CurrentNode().appendChild(new_element);
	return new_element;
}


QString CXMLBase::RootName() const
{
	return Root().tagName(); 
}

void CXMLBase::Save(QTextStream &stream) const
{
	stream << m_DomDoc->toString();
}

QDomNode CXMLBase::CurrentNode()
{
	return m_CurrentNode;
}

const QDomNode CXMLBase::CurrentNode() const
{
	return m_CurrentNode;
}

void CXMLBase::CurrentNode(QDomNode current) const
{
	m_CurrentNode = current;
}

//QDomNode CXMLBase::FirstChild(QDomNode node) const
//{
//	return node.firstChild();
//}
//
//QDomNode CXMLBase::LastChild(QDomNode node) const
//{
//	return node.lastChild();
//}
//
//QDomNode CXMLBase::NextSibling(QDomNode node) const
//{
//	return node.nextSibling();
//}
//
//QDomNode CXMLBase::PreviousSibling(QDomNode node) const
//{
//	return node.previousSibling();
//}


bool CXMLBase::AddAttribute(const QString &parent, const QString &name, const QString &value)
{
	QDomElement node = FindElement(parent);
	if(node.isNull())
		return false;

	node.setAttribute(name, value);
	return true;
}

bool CXMLBase::AddAttribute(const QString &parent, const QString &name, bool value)
{
	if(value)
		return AddAttribute(parent, name, QString("true"));

	return AddAttribute(parent, name, QString("false"));
}


bool CXMLBase::AddAttribute(const QString &parent, const QString &name, int value)
{
	QDomElement node = FindElement(parent);
	if(node.isNull())
		return false;

	node.setAttribute(name, value);
	return true;
}

bool CXMLBase::AddAttribute(const QString &parent, const QString &name, const double &value)
{
	QDomElement node = FindElement(parent);
	if(node.isNull())
		return false;

	node.setAttribute(name, value);
	return true; 
}

bool CXMLBase::AddAttribute(const QString &name, const QString &value)
{
	QDomNode node = CurrentNode();
	if(!node.isElement() || node.isNull())
		return false;

	node.toElement().setAttribute(name, value);
	return true;
}

bool CXMLBase::AddAttribute(const QString &name, int value)
{
	QDomNode node = CurrentNode();
	if(!node.isElement() || node.isNull())
		return false;

	node.toElement().setAttribute(name, value);
	return true; 
}

bool CXMLBase::AddAttribute(const QString &name, const double &value, int nPrecision /*= 20*/)
{
	QString str;
	str.setNum(value, 'g', nPrecision);
	return AddAttribute(name, str);
	
}

bool CXMLBase::AddAttribute(const QString &name, bool value)
{
	if(value)
		return AddAttribute(name, QString("true"));
	
	return AddAttribute(name, QString("false"));
}

//QDomNode CXMLBase::Child(const QString &name) const
//{
//	return m_CurrentNode.namedItem(name);
//}
