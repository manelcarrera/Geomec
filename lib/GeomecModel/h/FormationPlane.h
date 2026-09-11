// FormationPlane.h: interface for the CFormationPlane class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_FORMATION_PLANE)
#define _FORMATION_PLANE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000



class CHexaFormation;

#include "colornode.h"
#include "ISurface.h"
#include "GeomecModelVisitor.h"

class CFormationPlane : public CColorNode  
{
	class CPlaneSurface;

public:
	class plane_face : public geo::IFace
	{
//		geo::CPoint m_point[4];
		const CPlaneSurface &m_surface;
		int m_pointindices[4];
	public:
		plane_face(const CPlaneSurface &surface, int p1, int p2, int p3, int p4);
		virtual const geo::IPoint &Point(int nIndex) const;
		virtual void Point(int nIndex, const geo::IPoint &pt);//Substitute point at nIndex with pt.....
		virtual int NrOfPoints() const;
		virtual double Size() const; // returns volume, area or length (depending on type)
		virtual geo::CValue InterpolateValue(const geo::IPoint &point, const std::vector<geo::CValue> &values) const;
		int PointIndex(int nIndex) const;
		virtual size_t Order() const { return 1; }
	};
	enum ePlaneType { FP_BOTTOM, FP_MIDDLE, FP_TOP };
private:
	class CPlaneSurface : public geo::ISurface
	{
		std::vector<const geo::IFace*> m_vcFaces;
		std::vector<const geo::IPoint*> m_vcPoints; // refers to entries in m_mpPoints
		typedef std::map<geo::CPoint, int> TPointMap; // map points to their indices in the vector
		TPointMap m_mpPoints; // to make sure points are added only once
		typedef std::map<int, std::vector<const geo::IFace*> > TPointToFaceMap; // maps point indices to faces
		TPointToFaceMap m_mpPointToFace;
		typedef std::map<const geo::IFace*, std::vector<int> > TFaceToPointsMap; // maps face to the point indices
		TFaceToPointsMap m_mpFaceToPoints;

		std::vector<int> InsertPoints(const std::vector<geo::CPoint> &points);
		void InsertFace(const std::vector<int> &pointindices, const geo::IFace *pFace);
	protected:
		virtual geo::IPoint &PointAt(int nIndex);
	public:
		CPlaneSurface();
		virtual ~CPlaneSurface();
		void CreateFace(const std::vector<geo::CPoint> &points);
		virtual const geo::CPtrArray<geo::IFace> FacesAt(const geo::IPoint &p) const;
		virtual const geo::CPtrArray<geo::IFace> FacesAtNode(const geo::IPoint &p) const;
		virtual const geo::IFace &Face(int nIndex) const;
		virtual int FaceSize() const;

		virtual const geo::IPoint& Point(int nIndex) const;
		virtual int PointSize() const;

		// get the node indices connected to the given element
		virtual std::vector<int> Nodes(const geo::IElement &element) const;
	};

	// Members
//	mutable geo::CPtrArray<geo::IFace> m_Faces;
  mutable geo::ISurface *m_pSurface;
	ePlaneType m_type;
	CHexaFormation& m_formation;

	class CPointLess {
	public:
		bool operator()(const geo::IPoint* p1, const geo::IPoint*p2)
		{ return *p1 < *p2; }
	};

	// Helpers
	bool Dirty() const;
	bool UpdatePlane() const;
public:
	// Construction / Destruction
	CFormationPlane(const CFormationPlane& rhs);
	CFormationPlane(ePlaneType type, CHexaFormation &formation);
	virtual ~CFormationPlane();

	bool operator==(const CFormationPlane& plane) const;
	CFormationPlane& operator=(const CFormationPlane& plane);

	virtual unsigned int IconId() const;
	virtual unsigned int TypeId() const;
	virtual QString TypeName() const;
	const geo::IObject& DisplayList(int nIndex) const;
	int DisplayListSize() const;

	virtual int NrOfFaces() const;
	virtual bool CanDestroy() const;

	CHexaFormation& Formation();
	const CHexaFormation& Formation() const;
	ePlaneType PlaneType() const;

	void Clear();
	const geo::ISurface& PlaneSurface() const;
	virtual bool Less(const CGraphNode &node) const;

	virtual void OnNeighbourModified(const CGraphNode &node, enum ModifiedHint uHint);
	virtual void OnNeighbourDeleted(const CGraphNode& node);

  ACCEPT_GEOMECMODELVISITORS(VisitFormationPlane);
};

typedef CSingleCommandTemplate<CFormationPlane> TFormationPlaneCommand;

#endif // !defined(_FORMATION_PLANE)

