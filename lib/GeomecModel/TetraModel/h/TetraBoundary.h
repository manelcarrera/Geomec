// TetraBoundary.h: interface for the CTetraBoundary class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TETRABOUNDARY_H__601D24B7_C60D_455B_98CE_1755B8E147B9__INCLUDED_)
#define AFX_TETRABOUNDARY_H__601D24B7_C60D_455B_98CE_1755B8E147B9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "InterfaceBoundary.h"
#include "BoundaryInterfaceDef.h"
#include "TetraHorizonBase.h"

namespace geo {
	class CInterfaceElement;
}

// This is main boundary of the model. If the boundary 
class CTetraSubBoundary;
class CTetraSuperHorizonEntry;
class CTetraHorizonBase;

namespace geo {
	class CSurfaceDesc;
	class CElementGroup;
}

class CTSSurface;
class CBoundaryInterfaceDef;

class CTetraBoundary : public CInterfaceBoundary  
{
private:
	CLengthQuantity m_distance;
	double m_dLoadedDistance;
	CTetraSubBoundary* m_pSubBoundary;
	mutable CTSSurface* m_side_surface[4];
	CTetraSuperHorizonEntry* m_pSuperHorizonEntry;

private:
	void AddPointsFromSurface(const geo::ISurface& surface, geo::CArray<geo::CPoint>& arPoint, int nIndex) const;

public:
//	enum SIDE_SURFACE_B { SS_FRONT = 0, SS_BACK, SS_LEFT, SS_RIGHT };
	// Construction
	CTetraBoundary(CTetraSubBoundary& boundary);
	CTetraBoundary(const CTetraBoundary& rhs);
	~CTetraBoundary();

	bool operator==(const CTetraBoundary& rhs) const;
	CTetraBoundary& operator=(const CTetraBoundary& rhs);
	const geo::IObject& DisplayList(int nIndex) const;
	int DisplayListSize() const;

private:
	void CreateInterfaceElements();
	void ElementsFromSurfDesc(const geo::CSurfaceDesc &sd);

public:
	TMinMax BestFit() const;
	virtual TMinMax SnapToGrid(const TMinMax& minmax) const;
  virtual TRotated SnapToGrid(const TRotated& rotated) const;

	// Top and bottom horizon. The top and bottom is could return zero when they are
	// simply not defined by the user. This must be handled by the code.
	const CTetraHorizonBase* TopHorizon() const;
	CTetraHorizonBase* TopHorizon();
	const CTetraHorizonBase* BottomHorizon() const;
	CTetraHorizonBase* BottomHorizon();

	// Is Super model?
	bool IsSuperModel() const;
	bool IsSuperModelValid() const;

	// Distance to subboundary
	bool DistanceToSubBoundary(const double& dDistance, CQuantity::UNIT unit);
	const CLengthQuantity& DistanceToSubBoundary() const;

	// Side surfaces of the boundary
	int SideSurfaceSize() const;
	// input surface
	const geo::ISurface& SideSurface(int nIndex) const;
	// output surface
	const geo::CSurfaceDesc& SideSurfaceDesc(int nIndex) const;
	// Side surface name
	QString SideSurfaceName(int nIndex) const;

	virtual void LoadStream(TSTREAM& stream, CStreamVersion& version, TPROGRESS& progress);
	virtual void SaveStream(TSTREAM& stream, TPROGRESS& progress);
	virtual void OnNeighbourModified(const CGraphNode& node, enum ModifiedHint uHint);
	virtual void OnNeighbourDeleted(const CGraphNode &node);

	// Top and bottom horizon
	virtual const CTetraHorizonBase &GetTopHorizon() const;
	virtual const CTetraHorizonBase &GetBottomHorizon() const;

  virtual std::vector<const geo::ISurface*> GetSideMeshSurfaces() const;

	// Export surfaces of boundary
  void ExportSurfaces(const QString& fileName, const CUnitNode& unitNode);
	bool CanExportSurface() const;

  ACCEPT_GEOMECMODELVISITORS(VisitTetraBoundary);

protected:
	// Side surface generation
	void InvalidateSideSurfaces();
	void GenerateSurface(int nIndex) const;
	virtual bool OnSet(const TMinMax& minmax);
};

#endif // !defined(AFX_TETRABOUNDARY_H__601D24B7_C60D_455B_98CE_1755B8E147B9__INCLUDED_)
