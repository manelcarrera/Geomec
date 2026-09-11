// HexaBoundary.h: interface for the CHorizonBoundary class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HEXABOUNDARY_H__21D8AAA9_B215_42BF_92C5_406E5114ED5C__INCLUDED_)
#define AFX_HEXABOUNDARY_H__21D8AAA9_B215_42BF_92C5_406E5114ED5C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "InterfaceBoundary.h"
#include "HexaHorizon.h"
#include "GeomecModelVisitor.h"

class CHexaMesh;
class CBoundaryInterfaceDef;
class CHexaMainMeshRegion;

class CHexaBoundary : public CInterfaceBoundary
{
public:
	CHexaBoundary(const geo::IPoint& ptMin,
				  const geo::IPoint& ptMax,
				  CFemAppModel& model,
				  BOUNDARY_STATE state = DEFAULT_DEFINED);
  ~CHexaBoundary();

	TMinMax BestFit() const;

	virtual TMinMax SnapToGrid(const TMinMax& minmax) const;
  virtual TRotated SnapToGrid(const TRotated& rotated) const;

	// Events
	virtual void OnNewNeighbour(const CGraphNode &node);
	virtual void OnNeighbourModified(const CGraphNode &node, enum ModifiedHint uHint);

	// Top and bottom horizon
	virtual const CHexaHorizon &GetTopHorizon() const;
	virtual const CHexaHorizon &GetBottomHorizon() const;

  virtual std::vector<const geo::ISurface*> GetSideMeshSurfaces() const;

  virtual void LoadStream(TSTREAM& stream, CStreamVersion& version, TPROGRESS& progress);

  virtual bool CanRotate() const;

  ACCEPT_GEOMECMODELVISITORS(VisitHexaBoundary);

protected:
	virtual bool OnSet(const TMinMax& minmax);
  virtual bool OnSet(const TRotated& rotated);

private:
	void OnUpdateBoundary();
	const CHexaMesh &Mesh() const;
  CHexaMainMeshRegion* MainMeshRegion() const;

  class CMinMaxLess
  {
  public:
    bool operator()(const geo::IObject* pL, const geo::IObject* pR)
    {
      geo::CPoint Lmin = pL->Min();
      geo::CPoint Rmin = pR->Min();
      if(Lmin.X() < Rmin.X())
        return true;
      if(Lmin.X() > Rmin.X())
        return false;
      if(Lmin.Y() < Rmin.Y())
        return true;
      if(Lmin.Y() > Rmin.Y())
        return false;
      if(Lmin.Z() < Rmin.Z())
        return true;
      if(Lmin.Z() > Rmin.Z())
        return false;

      geo::CPoint Lmax = pL->Max();
      geo::CPoint Rmax = pR->Max();
      if(Lmax.X() < Rmax.X())
        return true;
      if(Lmax.X() > Rmax.X())
        return false;
      if(Lmax.Y() < Rmax.Y())
        return true;
      if(Lmax.Y() > Rmax.Y())
        return false;
      if(Lmax.Z() < Rmax.Z())
        return true;

      return false;
    }
  };
};

#endif // !defined(AFX_HEXABOUNDARY_H__21D8AAA9_B215_42BF_92C5_406E5114ED5C__INCLUDED_)
