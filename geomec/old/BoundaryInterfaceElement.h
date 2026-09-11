// BoundaryInterfaceElement.h: interface for the CBoundaryInterfaceElement class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BOUNDARYINTERFACEELEMENT_H__AC55D701_D620_40B1_B0A9_360271A21498__INCLUDED_)
#define AFX_BOUNDARYINTERFACEELEMENT_H__AC55D701_D620_40B1_B0A9_360271A21498__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CTetraMesh;
class CTetraBoundary;

class CBoundaryInterfaceElement : public geo::IElement
{
	geo::IMesh &m_Mesh;
	const geo::IFace &m_ParentFace;
	std::vector<int> m_vcNodes;
	std::vector<geo::CLine> m_vcLines;
	int m_nIndex;
public:
	
	CBoundaryInterfaceElement(geo::IMesh &mesh,const geo::IFace &face, CTetraBoundary &bound);
	CBoundaryInterfaceElement(geo::IMesh &mesh,
							  const geo::IFace &face,
							  int nIdx1,
							  int nIdx2,
							  int nIdx3,
							  int nIdx4,
							  int nIdx5,
							  int nIdx6);


	virtual ~CBoundaryInterfaceElement();

	virtual const geo::IPoint &Point(int nIndex) const;
	virtual void Point(int nIndex, const geo::IPoint &pt);//Substitute point at nIndex with pt.....
	virtual int NrOfPoints() const;

	virtual int Order() const;

	virtual int NrOfLines() const;
	virtual const geo::ILine& Line(int nIndex) const;

	virtual double RepresentativeLength() const;

	virtual const geo::IElementSet* IndexingElementSet() const;
	virtual int Index() const;

	virtual int PointIndex(int nIndex) const;

	virtual int NrOfNodes() const;
	virtual const geo::INode& Node(int nIndex) const;
	virtual void Node(int nIndex, const geo::IPoint& point);

	virtual std::string Type() const;
	virtual bool Contains(const geo::IPoint &point, bool bIncludeEdge) const;

	inline const geo::IFace &ParentFace() const { return m_ParentFace; }
};

#endif // !defined(AFX_BOUNDARYINTERFACEELEMENT_H__AC55D701_D620_40B1_B0A9_360271A21498__INCLUDED_)
