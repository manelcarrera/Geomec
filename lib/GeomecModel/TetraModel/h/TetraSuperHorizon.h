// TetraSuperHorizon.h: interface for the CTetraSuperHorizon class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TETRASUPERHORIZON_H__DB893AF4_9454_46B8_9BCC_335AA8A181E1__INCLUDED_)
#define AFX_TETRASUPERHORIZON_H__DB893AF4_9454_46B8_9BCC_335AA8A181E1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "TetraHorizonBase.h"

class CTetraModel;
class CTetraSuperHorizon : public CTetraHorizonBase {
public:
  CTetraSuperHorizon(CFemAppModel &model);
  CTetraSuperHorizon(const QString &sName, const double &dDepth, CFemAppModel &model, bool bAttachToEntry = true);
  CTetraSuperHorizon(CSurfaceBase &surface, CFemAppModel &model, bool bSlip, bool bAttachToEntry = true);
  virtual ~CTetraSuperHorizon();
  virtual void LoadStream(TSTREAM &stream, CStreamVersion &version, TPROGRESS &progress);
  virtual bool IsDepthValid(const double &depth, const CLengthQuantity::UNIT unit = CLengthQuantity::SI_UNIT);
  virtual bool CanConnectItem(const CGraphNode &item) const;
  virtual bool ConnectItem(const CGraphNode &item);
  virtual bool CanSlip() const;
  virtual bool Less(const CGraphNode &node) const;
  virtual bool IsTopHorizon() const;

  ACCEPT_GEOMECMODELVISITORS(VisitTetraSuperHorizon);
};

class CTetraSuperHorizonEntry : public CStorageNodeEntry<CTetraSuperHorizon> {
public:
  typedef CSingleCommandTemplate<CTetraSuperHorizonEntry> TTetraSuperHorizonEntryCommand;

private:
  bool SurfaceIntersection(const geo::ISurface &surface1, const geo::ISurface &surface2) const;
  bool HorizonIntersection(const geo::ISurface &surface, const C3DHorizon &horizon) const;

public:
  CTetraSuperHorizonEntry(CTetraModel &model);
  bool CanAppendSurface(const geo::ISurface &surface) const;
  virtual bool CanConnectItem(const CGraphNode &item) const;
  virtual bool ConnectItem(const CGraphNode &item);

  ACCEPT_GEOMECMODELVISITORS(VisitTetraSuperHorizonEntry);
};

#endif // !defined(AFX_TETRASUPERHORIZON_H__DB893AF4_9454_46B8_9BCC_335AA8A181E1__INCLUDED_)
