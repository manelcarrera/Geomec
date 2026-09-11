 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */
// ElementGroup.h: interface for the CElementGroup class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ELEMENTGROUP_H__D26FF206_7735_4381_8D78_809942D2CD73__INCLUDED_)
#define AFX_ELEMENTGROUP_H__D26FF206_7735_4381_8D78_809942D2CD73__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "IElementSet.h"

#include "GeometryExports.h"

namespace geo {
class IMesh;
class GEOMETRY_EXPORT  CElementGroup : public IElementSet  
{
	std::vector<IElement*>   m_vcElement;
	mutable std::vector<int> m_vcNode;
	IMesh &m_mesh;
  bool m_bVisible;
	virtual IPoint &PointAt(int nIndex);
	void createPointBuffer() const;
public:
	CElementGroup(IMesh& mesh, bool bVisible = true);
	virtual ~CElementGroup();

	virtual bool AddMeshElement(IElement& elem);

	virtual const IPoint &Point(int nIndex) const;
	virtual int PointSize() const;

	virtual const IElement &Element(int nIndex) const;
	virtual IElement& Element(int nIndex);

	virtual int ElementSize() const;
	virtual std::vector<int> ElementsAt(const IPoint &point) const;
	using IElementSet::ElementsAt;

	// get the node indices connected to the given element
	virtual std::vector<int> Nodes(const IElement &element) const;

	virtual void AssertValid() const;

  bool isVisible() const;
  void setVisible(bool bVisible);

};

}

#endif // !defined(AFX_ELEMENTGROUP_H__D26FF206_7735_4381_8D78_809942D2CD73__INCLUDED_)
