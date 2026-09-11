// XMLBase.h: interface for the XMLBase class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_XMLBASE_H__468AE10E_6038_4860_BD5A_C02CB804DDDE__INCLUDED_)
#define AFX_XMLBASE_H__468AE10E_6038_4860_BD5A_C02CB804DDDE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include <qdom.h>
#include <QTextStream>
#include <qfile.h>
//class QDomDocument;
//class QDomNode;
//class QDomElement;
class CXMLBase  
{
private:
	QDomDocument *m_DomDoc;
	mutable QDomNode m_CurrentNode;
	
public:
	CXMLBase(const QString &strRoot);
	CXMLBase(const QFile &file);
	virtual ~CXMLBase();

	QDomElement Root() const;

	/* adds an element in parent, is added behind any other elements present in parent
	   also the first element in the xml tree named parent will be used. returns the newly created element*/
	QDomNode AddElement(const QString &parent, const QString &element);
	
	/* same as previous but now the element is added to m_CurrentNode if m_CurrentNode is an element. 
	   returns the newly created element*/
	QDomNode AddElement(const QString &element);

	/* functions to add attributes to an element. the parent string is the tagname of the element
	   in which the attribute will have to be created. returns true the attribute was inserted*/
	bool AddAttribute(const QString &parent, const QString &name, const QString &value);
	bool AddAttribute(const QString &parent, const QString &name, int value);
	bool AddAttribute(const QString &parent, const QString &name, const double &value);
	bool AddAttribute(const QString &parent, const QString &name, bool value);

	/* same as previous but now the attributes are created in m_CurrentNode if m_CurrentNode is an element 
	   returns true the attribute was inserted*/
	bool AddAttribute(const QString &name, const QString &value);
	bool AddAttribute(const QString &name, int value);
	bool AddAttribute(const QString &name, const double &value, int nPrecision = 20);
	bool AddAttribute(const QString &name, bool value);

	/* returns the name of the root */
	QString RootName() const;
	
	void Save(QTextStream &stream) const;

	QDomElement FindElement(const QString element) const;
	QDomNode CurrentNode();
	const QDomNode CurrentNode() const;
	void CurrentNode(QDomNode current) const;
//	QDomNode FirstChild(QDomNode node) const;
//	QDomNode NextSibling(QDomNode node) const;
//	QDomNode PreviousSibling(QDomNode node) const;
//	QDomNode LastChild(QDomNode node) const;


	/* Returns the first Child with the name supplied, in the current node */
//	QDomNode Child(const QString &name) const;

};

#endif // !defined(AFX_XMLBASE_H__468AE10E_6038_4860_BD5A_C02CB804DDDE__INCLUDED_)
