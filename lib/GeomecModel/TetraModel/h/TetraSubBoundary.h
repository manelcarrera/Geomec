// TetraSubBoundary.h: interface for the CTetraSubBoundary class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TETRASUBBOUNDARY_H__F2E84A5B_2E23_4694_9A02_AD2A9561B7A5__INCLUDED_)
#define AFX_TETRASUBBOUNDARY_H__F2E84A5B_2E23_4694_9A02_AD2A9561B7A5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

namespace geo {
  class CSurfaceDesc;
}

// Class represents all surfaces of the boundary ....
class CTetraSubHorizon;
class CTetraModel;
class CSurfaceBase;

#include "colornode.h"
#include "GeomecModelVisitor.h"

class CTetraSubBoundary : public CColorNode
{
  // This drawdef makes the boundary always transparent (Fill is suppressed)
  class CBoundaryDrawDef : public CColorNode::CDrawDef
  {
  public:
    CBoundaryDrawDef(const COpenGLNode& node)
      :CColorNode::CDrawDef(node) {}
    DrawDecisionBool PolyFillFront() const { return std::make_pair(true, false); }
    DrawDecisionBool PolyFillBack() const { return std::make_pair(true, false); }
  };

  typedef std::vector<const geo::IObject*> TDisplayVec;
  const TDisplayVec& GenerateDisplayList() const;
public:
  class CHorizonPlaceHolder : public COpenGLNode
  {
    CTetraSubHorizon* m_pHorizon;
    CTetraSubBoundary& m_boundary;
    CFemAppModel& m_model;
  public:
    CHorizonPlaceHolder(unsigned int uName, CTetraSubBoundary& boundary);
    // Horizon access
    const CTetraSubHorizon* Horizon() const;
    CTetraSubHorizon* Horizon();

    virtual bool Empty() const;
    virtual long SavedItems() const;
    virtual TColor Color() const;

    virtual int DisplayListSize() const;
    virtual const geo::IObject& DisplayList(int nIndex) const;

    // Graph overrides
    virtual unsigned int IconId() const;
    virtual unsigned int TypeId() const;

    // Can we disconnect?
    virtual bool CanDisconnectItem(const CGraphNode& item) const;

    // Connection management
    virtual void OnNewNeighbour(const CGraphNode &node);
    virtual void OnNeighbourModified(const CGraphNode &node, enum ModifiedHint uHint);
    virtual void OnNeighbourDeleted(const CGraphNode &node);
    virtual bool CanConnectItem(const CGraphNode &item) const;
    virtual bool Less(const CGraphNode &node) const;
  };

  typedef enum
  {
    DRAW_INIT,
    DRAW_ALL,
    DRAW_SIDES
  } TDrawMode;

private:
  friend class CHorizonPlaceHolder;
  typedef std::vector<CSurfaceBase*> TSurfaceVec;
  CHorizonPlaceHolder* m_pTop;
  TSurfaceVec m_vcSideSurface;
  CHorizonPlaceHolder* m_pBottom;
  void UpdateHorizonSurfaces(CHorizonPlaceHolder& horizon);
  TDrawMode m_drawmode;
  mutable TDisplayVec m_vcDisplayList;

public:
  CTetraSubBoundary(CTetraModel& model);
  virtual ~CTetraSubBoundary();

  void setDrawMode(TDrawMode drawmode);

  // Top and bottom horizon
  const CHorizonPlaceHolder& TopHorizon() const;
  CHorizonPlaceHolder& TopHorizon();
  const CHorizonPlaceHolder& BottomHorizon() const;
  CHorizonPlaceHolder& BottomHorizon();

  // Side surface
  int SideSurfaceSize() const;
  const CSurfaceBase& SideSurface(int nIndex) const;
  CSurfaceBase& SideSurface(int nIndex);
  const geo::CSurfaceDesc& SideSurfaceDesc(int nIndex) const;

  // Can we disconnect?
  virtual bool CanDisconnectItem(const CGraphNode& item) const;

  virtual int DisplayListSize() const;
  virtual const geo::IObject& DisplayList(int nIndex) const;		// Always ...
  virtual CDrawDef* OnCreateDrawDef(const geo::IObject& object) const;
  virtual unsigned int IconId() const;
  virtual unsigned int TypeId() const;
  virtual QString TypeName() const;
  virtual bool Empty() const;
  virtual long SavedItems() const;
  virtual void LoadStream(TSTREAM& stream, CStreamVersion& version, TPROGRESS& progress);
  virtual void SaveStream(TSTREAM& stream, TPROGRESS& progress);

  // Some graph handlers
  virtual void OnNewNeighbour(const CGraphNode &node);
  virtual void OnNeighbourModified(const CGraphNode &node, enum ModifiedHint uHint);
  virtual void OnNeighbourDeleted(const CGraphNode &node);
  virtual bool CanConnectItem(const CGraphNode &item) const;
  virtual bool Less(const CGraphNode &node) const;

  void MeshInvalidated();

  ACCEPT_GEOMECMODELVISITORS(VisitTetraSubBoundary);
};

#endif // !defined(AFX_TETRASUBBOUNDARY_H__F2E84A5B_2E23_4694_9A02_AD2A9561B7A5__INCLUDED_)
