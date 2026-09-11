// TetraHorizonBase.h: interface for the CTetraHorizonBase class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TETRAHORIZONBASE_H__6BFB0FAC_D2AE_4AFD_9EC6_FEA8AA91B3B2__INCLUDED_)
#define AFX_TETRAHORIZONBASE_H__6BFB0FAC_D2AE_4AFD_9EC6_FEA8AA91B3B2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "3DHorizon.h"

namespace geo {
  class CSurfaceDesc;
}

class CTetraHorizonBase : public C3DHorizon
{
private:
  bool m_bFlip;
  // Display Surfaces
  std::vector<const geo::ISurface*> DisplaySurfaces() const;
  mutable geo::CElementGroup* m_pInterfaceElements;

public:
  // Construction
  CTetraHorizonBase(CFemAppModel& model);
  CTetraHorizonBase(CSurfaceBase &surface, CFemAppModel& model, const bool bSlip = false, bool bAttachToEntry = true);
  CTetraHorizonBase(const QString& strInstanceName, const double dDepth, CFemAppModel& model, bool bAttachToEntry = true);
  CTetraHorizonBase(const QString& strInstanceName, CFemAppModel& model, bool bAttachToEntry = true);
  CTetraHorizonBase(const C3DHorizon &rhs);
  virtual ~CTetraHorizonBase();

  void init();

  bool operator==(const CTetraHorizonBase& rhs) const;
  CTetraHorizonBase& operator=(const CTetraHorizonBase& rhs);

  virtual const geo::CElementGroup* InterfaceElementGroup() const;

  // Flip or not
  bool Flip() const;
  void Flip(bool bFlip);

  // Output Surfaces
  int OutputSurfaceSize() const;
  const geo::CSurfaceDesc& OutputSurface(int nIndex) const;

  virtual int MeshedSurfaceSize() const;
  virtual const geo::ISurface& MeshedSurface(int nIndex) const;

  // Display list output
  virtual const geo::IObject& DisplayList(int nIndex) const;
  virtual int DisplayListSize() const;

  // Messaging from mesh
  virtual void OnNeighbourModified(const CGraphNode& node, enum ModifiedHint uHint);
  virtual void LoadStream(TSTREAM& stream, CStreamVersion& version, TPROGRESS& progress);
  virtual void SaveStream(TSTREAM& stream, TPROGRESS& progress);

//	virtual int MeshFaceSize() const;
//	virtual const geo::IFace &MeshFace(int nIndex) const;
  
  virtual int BodyFaceSize() const;
  virtual const geo::IFace &BodyFace(int nIndex) const;

  virtual bool CanDisconnectItem(const CGraphNode& item) const;
  virtual bool Destroy();

  ACCEPT_GEOMECMODELVISITORS(VisitTetraHorizonBase);
};

#endif // !defined(AFX_TETRAHORIZONBASE_H__6BFB0FAC_D2AE_4AFD_9EC6_FEA8AA91B3B2__INCLUDED_)
