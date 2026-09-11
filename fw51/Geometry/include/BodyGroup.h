 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */
// BodyGroup.h: interface for the CBodyGroup class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BODYGROUP_H__FE818D38_E6A2_4634_B15F_788BA21ED27D__INCLUDED_)
#define AFX_BODYGROUP_H__FE818D38_E6A2_4634_B15F_788BA21ED27D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <map>
#include "IVolume.h"
#include "ISurface.h"
#include "DispatchVisitorBase.h"

#include "GeometryExports.h"

namespace geo { 
// Group of connected bodies in the mesh. 
class IMesh;
class IFace;
class GEOMETRY_EXPORT  CBodyGroup : public IVolume
{
	IMesh &m_mesh;
	std::vector<int> m_vcElement;
	std::vector<int> m_vcNode;

public:
	typedef std::vector<const IBody*> TBodyVec;

private:
	typedef std::map<const IPoint*, TBodyVec> TPointMap;
	TPointMap m_mpPoint;

protected:
	virtual IPoint &PointAt(int nIndex);

public:
	// You can optional add side surfaces to the bodygroups.
	class GEOMETRY_EXPORT CSideSurface : public ISurface
	{
		std::vector<const IFace*> m_vcFace;
		typedef std::map<const IPoint*, int, ICoordinate::CCoordinateLess> TPointMap;
		TPointMap m_mpPoint;
		CBodyGroup& m_body_group;
		typedef std::vector<int> TFaceIndexVec;
		typedef std::pair<const IPoint*, TFaceIndexVec> TSurfacePoint;
		std::vector<TSurfacePoint> m_vcPoint;
	
	protected:
		virtual IPoint &PointAt(int nIndex);
	public:
		// Side surface constructor
		CSideSurface(CBodyGroup& body_group);
		virtual ~CSideSurface();

		// Construction
		bool AddFace(const IFace& face);

		const CBodyGroup& BodyGroup() const;
		// get the point at the given index
		virtual const IPoint& Point(int nIndex) const;
		// get the number of points in this surface
		virtual int PointSize() const;

		// get the node indices connected to the given element
		virtual std::vector<int> Nodes(const IElement &element) const;

		// get the face(s) at the given (arbitrary) point
		virtual const CPtrArray <IFace> FacesAt(const IPoint &p) const;
		// get the face(s) at the given point,
		// which must be a node of the surface (e.g. returned by the Point function)
		virtual const CPtrArray <IFace> FacesAtNode(const IPoint &p) const;

    virtual void reserveFaceSize(size_t size);

		// get the face at the given index
		virtual const IFace &Face(int nIndex) const;
		// get the number of faces in this surface
		virtual int FaceSize() const;

    void ReplaceNode(const INode& oldNode, const INode& newNode);
	};

	friend class CSideSurface;
private:
	std::vector<CSideSurface*> m_vcSurface;

public:
	// Construction / Destruction
	CBodyGroup(IMesh& mesh);
	virtual ~CBodyGroup();

	// Add a mesh body
	bool AddBody(const IBody& body);
  void reserveBodySize(size_t size);

	void Clear();

	// get the mesh
	const IMesh& Mesh() const;
	IMesh& Mesh();

	virtual const IBody& Body(int nIndex) const;
	virtual int BodySize() const;

	virtual const IPoint &Point(int nIndex) const;
	virtual int PointSize() const;

	virtual void AssertValid() const;

	// Point is member of the mesh
	// Return value : Bodies adjacent to the point
	const std::vector<const IBody*>& PointToBody(const IPoint &point) const;

	virtual std::vector<int> Nodes(const IElement &element) const;

	// Side surfaces of the body
	int SideSurfaceSize() const;
	const CSideSurface& SideSurface(int nIndex) const;
	CSideSurface& SideSurface(int nIndex);

	// Acces to all edges of the faces
	virtual const IFace &EdgeFace(int Index) const;
	virtual int EdgeFaceSize() const;

	void ReplaceNode(IBody& body, int nOldNodeIndex, int nNewNodeIndex);

	virtual bool Visit(IVisitor &visitor)
	{
		assert(dynamic_cast<CVisitorBase *>(&visitor));
		return static_cast<CVisitorBase &>(visitor).HandleBodyGroup(*this);
	}
};

}

#endif // !defined(AFX_BODYGROUP_H__FE818D38_E6A2_4634_B15F_788BA21ED27D__INCLUDED_)
