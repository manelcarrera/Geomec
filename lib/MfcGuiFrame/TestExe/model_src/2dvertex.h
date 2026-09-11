// 2DVertex.h: interface for the C2DVertex class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_2DVERTEX_H__8D342819_56E1_4649_BA79_53335F6AEF1A__INCLUDED_)
#define AFX_2DVERTEX_H__8D342819_56E1_4649_BA79_53335F6AEF1A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ModelContainer.h"
#include "ringfactory.h"
#include <map>

class C2DSegment;
class C2DModel;

class C2DVertex : public IModelObject
{
public:
	class CSegmentPoint : public geo::ISegmentPoint
	{
		C2DVertex& m_vertex;
		double m_coor[3];
	public:
		CSegmentPoint(C2DVertex& vertex);
		CSegmentPoint(const CSegmentPoint& rhs);

		C2DVertex& Vertex();
		const C2DVertex& Vertex() const;

		bool operator==(const CSegmentPoint& rhs) const;
		CSegmentPoint& operator=(const CSegmentPoint& rhs);

		virtual const double &X() const;		
		virtual const double &Y() const;		
		virtual const double &Z() const;		
		virtual void X(const double &dX);		
		virtual void Y(const double &dY);		
		virtual void Z(const double &dZ);	
		virtual int SegmentSize() const;
		virtual const geo::ISegment& Segment(int nIndex) const;
		virtual void AssertValid() const;
		virtual bool Empty() const;
	};
private:
	friend class CSegmentPoint;
	std::vector<C2DSegment*> m_vcContainer;
	CSegmentPoint m_point;
	std::set<IModelObject*> Identifier(const std::vector<const C2DVertex*>& vcVertex) const;
protected:
	virtual void referenceInserted(IModelObject& ref);
	virtual void referenceRemoved(IModelObject& ref);
public:
	C2DVertex(const geo::IPoint& point, IModelObject& parent);
	C2DVertex(const double& dT, const double& dZ, IModelObject& parent);
	C2DVertex(const C2DVertex& rhs);
	C2DVertex(IModelObject& parent);
	virtual ~C2DVertex();

	bool operator==(const C2DVertex& rhs) const;
	C2DVertex& operator=(const C2DVertex& rhs);

	const geo::ISegmentPoint& Point() const;
	geo::ISegmentPoint &Point();

	int SegmentSize() const;
	const C2DSegment &Segment(int nIndex) const;
	C2DSegment &Segment(int nIndex);

//	virtual UINT IconId() const;
//	virtual UINT TypeId() const;

	virtual int geometrySize() const;
	virtual const geo::IObject& geometry(int nIndex) const;		// Always ...

	typedef QRgb TColor;
	virtual TColor Color() const;
/*
	virtual BOOL Empty() const;
	virtual long SavedItems() const;

	virtual void LoadStream(TSTREAM& stream, CVersion& version, TPROGRESS& progress);
	virtual void SaveStream(TSTREAM& stream, TPROGRESS& progress);
*/

/*
	virtual bool Properties();
*/
	virtual bool CanDestroy() const;
	virtual bool Destroy();

	bool CanMove(const geo::IPoint& point) const;
	bool CanMove(const double& dT, const double& dZ) const;
	bool Move(const geo::IPoint& point);
	bool Move(const double& dT, const double& dZ);
};

class C2DDocument;
class C2DVertexContainer : public CModelContainer<C2DVertex>
{
	typedef std::map<const geo::IPoint*, C2DVertex*, geo::ICoordinate::CCoordinateLess> TVertexMap;
	TVertexMap m_vertices;
protected:
	virtual void childInserted(IModelObject& child);	//*!< A child is inserted and the storage is invalidated.
	virtual void childRemoved(IModelObject& child);		//*!< A child is remove the tree and the storage is invalidated.
public:
	C2DVertexContainer(const char* name, C2DDocument& document);

	C2DVertex* NearTo(const geo::IPoint& point) const;
};

#endif // !defined(AFX_2DVERTEX_H__8D342819_56E1_4649_BA79_53335F6AEF1A__INCLUDED_)
