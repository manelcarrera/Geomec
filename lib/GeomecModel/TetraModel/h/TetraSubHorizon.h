// TetraSubHorizon.h: interface for the CTetraSubHorizon class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TETRASUBHORIZON_H__D2DF184A_1BD3_4AA6_8FD2_A4CA05980D42__INCLUDED_)
#define AFX_TETRASUBHORIZON_H__D2DF184A_1BD3_4AA6_8FD2_A4CA05980D42__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "TetraHorizonBase.h"

// ##ModelId=3E3E5F010000
class CTetraSubHorizon : public CTetraHorizonBase {
public:
  void AttachToEntry();
  // ##ModelId=3E3E5F010002
  CTetraSubHorizon(CFemAppModel &model);
  // ##ModelId=3E3E5F010004
  CTetraSubHorizon(const QString &sName, bool bSlip, CFemAppModel &model, bool bAttachToEntry = true);
  // ##ModelId=3E3E5F010010
  CTetraSubHorizon(CSurfaceBase &surface, CFemAppModel &model, bool bSlip, bool bAttachToEntry = true);

  CTetraSubHorizon(const CTetraSubHorizon &rhs);
  // ##ModelId=3E3E5F010013
  virtual ~CTetraSubHorizon();
  // ##ModelId=3E3E5F010015
  virtual unsigned int IconId() const;
  // ##ModelId=3E3E5F010019
  virtual bool CanConnectItem(const CGraphNode &item) const;
  // ##ModelId=3E3E5F01001C
  virtual void LoadStream(TSTREAM &stream, CStreamVersion &version, TPROGRESS &progress);
  virtual void SaveStream(TSTREAM &stream, TPROGRESS &progress);
  // ##ModelId=3E3E5F010023
  virtual unsigned int TypeId() const;
  virtual QString TypeName() const;
  virtual bool CanSlip() const;
  bool CanToggleSlip() const;
  void ToggleSlip();

  bool FaultsLess(const CGraphNode &node) const;
  virtual bool Less(const CGraphNode &node) const;
  virtual bool IsTopHorizon() const;

  virtual bool DoubleSidedFault() const;
  virtual bool IgnoreFault() const;

  virtual const CTetraSubHorizon *IntermediateFault() const;
  virtual const CTetraSubHorizon *MinusFault() const;
  virtual const CTetraSubHorizon *PlusFault() const;

  ACCEPT_GEOMECMODELVISITORS(VisitTetraSubHorizon);

protected:
  virtual void OnSlipToggled();

private:
  QString DSFNameBase(const QString &name) const;
  const CTetraSubHorizon *DSFault(const QString &name) const;
};

// ##ModelId=3E3E5F010025
class CTetraModel;
class CTetraSubHorizonEntry : public CStorageNodeEntry<CTetraSubHorizon> {
public:
  CTetraSubHorizonEntry(int nEntryId, unsigned int uIconId, unsigned int uNameId, CFemAppModel &model);
  virtual bool CanConnectItem(const CGraphNode &item) const;
  virtual bool ConnectItem(const CGraphNode &item);

  ACCEPT_GEOMECMODELVISITORS(VisitTetraSubHorizonEntry);
};

#endif // !defined(AFX_TETRASUBHORIZON_H__D2DF184A_1BD3_4AA6_8FD2_A4CA05980D42__INCLUDED_)
