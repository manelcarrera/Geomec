 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */
// Mesh.h: interface for the CMesh class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MESH_H__9A7F5F89_3B8C_469B_892B_EF45694A1883__INCLUDED_)
#define AFX_MESH_H__9A7F5F89_3B8C_469B_892B_EF45694A1883__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "IMesh.h"
#include "INode.h"

#include "GeometryExports.h"

namespace geo {

class GEOMETRY_EXPORT  CMesh : public IMesh  
{
	class GEOMETRY_EXPORT CNode : public INode
	{
		const IMesh& m_mesh;
		int m_nIndex;
		double m_coor[3];
		std::vector<IElement*> m_vcElement;

	public:
		CNode(const IMesh &mesh, int nIndex, const IPoint& point, int nElements = 0);
		virtual ~CNode();

		virtual const IMesh &Mesh() const;
		virtual int Index() const;
    void Index(int nIndex);

		// Coordinate access
		virtual const double &X() const;
		virtual const double &Y() const;
		virtual const double &Z() const;

		// Coordinate modification
		virtual void X(const double &dX);
		virtual void Y(const double &dY);
		virtual void Z(const double &dZ);

		virtual void AssertValid() const;
		virtual bool Empty() const;

		// Looping over elements
		virtual int AttachedElementSize() const;
		virtual const geo::IElement& AttachedElement(int nIndex) const;

		// Register element
		virtual int RegisterElement(geo::IElement& element);
		virtual bool UnregisterElement(geo::IElement& element);
	};

	std::vector<IElement*> m_vcElement;

  std::vector<const CTiedBodyTriangle *> m_vcTiedBodyTriangle;

	typedef std::map<const IPoint*, TNodeVec, ICoordinate::CCoordinateLess> TPointMap;
	TPointMap m_mpPoint;

	CNearestNeighbour m_vcPoint;

	virtual CNode* CreateMeshNode(int nIndex, const IPoint& point, int nElements);

protected:
	TNodeVec NearestPoint(const IPoint& point) const;

	// Access to non const point
	virtual IPoint &PointAt(int nIndex);

private:
  void BuildUniqueMap();

public:
	CMesh();
	virtual ~CMesh();

	// Registering elements and points in the mesher
	virtual int RegisterNode(const IPoint& point, bool bUnique = true, int nElements = 0);

	virtual int RegisterElement(IElement& element);
  virtual void reserveElementSize(size_t size);

  virtual int RegisterTiedBodyTriangle(int nIndex, const CTiedBodyTriangle& tiedBodyTriangle);
  virtual const CTiedBodyTriangle& TiedBodyTriangle(int nIndex);

  virtual void RemoveElement(int nIndex);
  virtual void RemoveNode(int nIndex);

	// Clear the mesh
	virtual void Clear();

	// mesh nodes
	virtual int NodeSize() const;
	virtual const INode& Node(int nIndex) const;
	virtual void Node(int nIndex, const IPoint& node);

	virtual TNodeVec NodeIndex(const IPoint& point);

	virtual const IElement &Element(int nIndex) const;
	virtual int ElementSize() const;
};

}

#endif // !defined(AFX_MESH_H__9A7F5F89_3B8C_469B_892B_EF45694A1883__INCLUDED_)
