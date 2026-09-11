#ifndef _EXPORTRESULTTREEBRANCH_H_
#define _EXPORTRESULTTREEBRANCH_H_

#include "ExportSelection.h"
#include "FormationPlane_Delegate.h"
#include "GraphEntry_Delegate.h"
#include "HexaFormation.h"
#include "HexaFormation_Delegate.h"
#include "HorizonBase_Delegate.h"
#include "NewWellPath.h"
#include "NewWellPath_Delegate.h"
#include "NonMeshedSurface.h"
#include "NonMeshedSurface_Delegate.h"
#include "PointSetEntryObserver.h"
#include "PointSet_Delegate.h"
#include "WellCasingRTCI.h"
#include "WellCasingRTCI_Delegate.h"
#include "WellModelEntryTypes.h"

// namespace for result export classes
namespace re {

typedef CGraphEntryTemp_Delegate<CHorizonBase> THorizonBaseEntry_Delegate;

// Slipping Horzion entry observer class
typedef CSelectionLeafObserver_Delegate<CHorizonBase, CHorizonBase_Delegate, THorizonBaseSelection, FALSE> THorizonObs;
typedef CSelectionBranchObserver_Delegate<THorizonBaseEntry, THorizonBaseEntry_Delegate, CHorizonBase, THorizonObs,
                                          CHorizonBase, THorizonBaseSelection, FALSE, FIXED_ITEM>
    THorizonBranchObs;

class CSlipHorizonEntryObserver : public THorizonBranchObs {
public:
  CSlipHorizonEntryObserver(THorizonBaseEntry &entry, THorizonBaseSelection &selection, CTreeCtrlBase &tcObject)
      : THorizonBranchObs(entry, tcObject, selection, &THorizonBaseSelection::IsSelected,
                          &THorizonBaseSelection::SetSelected) {
    Update();
  }

  virtual QString Text() const { return "Faults"; }
  virtual BOOL OnFilter(const CHorizonBase &horizon) const { return horizon.Slip(); }
};

// Non-slipping Horzion entry observer class
typedef CSelectionLeafObserver_Delegate<CHorizonBase, CHorizonBase_Delegate, THorizonBaseSelection, FALSE> THorizonObs;
typedef CSelectionBranchObserver_Delegate<THorizonBaseEntry, THorizonBaseEntry_Delegate, CHorizonBase, THorizonObs,
                                          CHorizonBase, THorizonBaseSelection, FALSE, FIXED_ITEM>
    THorizonBranchObs;

class CHorizonEntryObserver : public THorizonBranchObs {
public:
  CHorizonEntryObserver(THorizonBaseEntry &entry, THorizonBaseSelection &selection, CTreeCtrlBase &tcObject)
      : THorizonBranchObs(entry, tcObject, selection, &THorizonBaseSelection::IsSelected,
                          &THorizonBaseSelection::SetSelected) {
    Update();
  }

  virtual QString Text() const { return "Horizons"; }
  virtual BOOL OnFilter(const CHorizonBase &horizon) const { return !horizon.Slip(); }
};

// Formation plane entry observer class
typedef CSelectionLeafObserver_Delegate<CFormationPlane, CFormationPlane_Delegate, TFormationPlaneSelection, FALSE>
    TFormationPlaneObs;
typedef CSelectionBranchObserver_Delegate<CHexaFormation, CHexaFormation_Delegate, CFormationPlane, TFormationPlaneObs,
                                          CFormationPlane, TFormationPlaneSelection, FALSE, FIXED_ITEM>
    THexaFormationObs;
typedef CSelectionBranchObserver_Delegate<CHexaFormationEntry, CHexaFormationEntry_Delegate, CHexaFormation,
                                          THexaFormationObs, CFormationPlane, TFormationPlaneSelection, FALSE,
                                          FIXED_ITEM>
    THexaFormationEntryObs;

class CFormationPlaneEntryObserver : public THexaFormationEntryObs {
public:
  CFormationPlaneEntryObserver(CHexaFormationEntry &entry, TFormationPlaneSelection &selection, CTreeCtrlBase &tcObject)
      : THexaFormationEntryObs(entry, tcObject, selection, &TFormationPlaneSelection::IsSelected,
                               &TFormationPlaneSelection::SetSelected) {
    Update();
  }

  virtual unsigned int Icon() const { return IDI_FORMATIONPLANES; }
  virtual QString Text() const { return "Formation Planes"; }
};

// RTCI entry observer class
typedef CSelectionLeafObserver_Delegate<CWellCasingRTCI, CWellCasingRTCI_Delegate, TRTCISelection, FALSE> TRTCIObs;
typedef CSelectionBranchObserver_Delegate<CWellCasingRTCIEntry, CWellCasingRTCIEntry_Delegate, CWellCasingRTCI,
                                          TRTCIObs, CWellCasingRTCI, TRTCISelection, FALSE, FIXED_ITEM>
    TRTCIBranchObs;

class CRTCIEntryObserver : public TRTCIBranchObs {
public:
  CRTCIEntryObserver(CWellCasingRTCIEntry &entry, TRTCISelection &selection, CTreeCtrlBase &tcObject)
      : TRTCIBranchObs(entry, tcObject, selection, &TRTCISelection::IsSelected, &TRTCISelection::SetSelected) {
    Update();
  }

  virtual QString Text() const { return "RTCI"; }
};

// PointSet entry observer class
typedef CSelectionLeafObserver_Delegate<CPointSet, CPointSet_Delegate, TPointSetSelection, FALSE> TPointSetObs;
typedef CSelectionBranchObserver_Delegate<TPointSetEntry, TPointSetEntry_Delegate, CPointSet, TPointSetObs, CPointSet,
                                          TPointSetSelection, FALSE, FIXED_ITEM>
    TPointSetBranchObs;

class CPointSetEntryObserver : public TPointSetBranchObs {
public:
  CPointSetEntryObserver(TPointSetEntry &entry, TPointSetSelection &selection, CTreeCtrlBase &tcObject)
      : TPointSetBranchObs(entry, tcObject, selection, &TPointSetSelection::IsSelected,
                           &TPointSetSelection::SetSelected) {
    Update();
  }

  virtual QString Text() const { return "Point sets"; }

  virtual BOOL OnFilter(const CPointSet &point_set) const { return point_set.Dimension() == CPointSet::DIM_3D; }
};

// ElementSet entry observer class
typedef CSelectionLeafObserver_Delegate<IElementSet, IElementSet_Delegate, TElementSetSelection, TRUE> TElementSetObs;
typedef CSelectionBranchObserver_Delegate<TPointSetEntry, TPointSetEntry_Delegate, IElementSet, TElementSetObs,
                                          IElementSet, TElementSetSelection, FALSE, FIXED_ITEM>
    TElementSetBranchObs;

class CElementSetEntryObserver : public TElementSetBranchObs {
public:
  CElementSetEntryObserver(TPointSetEntry &entry, TElementSetSelection &selection, CTreeCtrlBase &tcObject)
      : TElementSetBranchObs(entry, tcObject, selection, &TElementSetSelection::IsSelected,
                             &TElementSetSelection::SetSelected) {
    Update();
  }

  virtual unsigned int Icon() const { return IDI_ELEMENT_SET; }
  virtual QString Text() const { return "Element sets"; }
  virtual BOOL OnFilter(const IElementSet &elementset) const {
    const CModelBase &model = static_cast<const CModelBase &>(elementset.Model());
    return (&elementset.ElementSet() != &model.Mesh().Mesh());
  }
};

typedef CSelectionLeafObserver_Delegate<CFormationBase, CFormationBase_Delegate, TFormationSelection, FALSE>
    TFormationObs;
typedef CSelectionBranchObserver_Delegate<TFormationBaseEntry, TFormationBaseEntry_Delegate, CFormationBase,
                                          TFormationObs, CFormationBase, TFormationSelection, FALSE, FIXED_ITEM>
    TFormationBranchObs;

class CFormationBaseEntryObserver : public TFormationBranchObs {
public:
  CFormationBaseEntryObserver(TFormationBaseEntry &entry, TFormationSelection &selection, CTreeCtrlBase &tcObject)
      : TFormationBranchObs(entry, tcObject, selection, &TFormationSelection::IsSelected,
                            &TFormationSelection::SetSelected) {
    Update();
  }
};

typedef CSelectionLeafObserver_Delegate<CNewWellPath, CNewWellPath_Delegate, TNewWellPathSelection, FALSE>
    TNewWellPathObs;
typedef CSelectionBranchObserver_Delegate<CNewWellPathEntry, CNewWellPathEntry_Delegate, CNewWellPath, TNewWellPathObs,
                                          CNewWellPath, TNewWellPathSelection, FALSE, FIXED_ITEM>
    TNewWellPathBranchObs;

class CNewWellPathEntryObserver : public TNewWellPathBranchObs {
public:
  CNewWellPathEntryObserver(CNewWellPathEntry &entry, TNewWellPathSelection &selection, CTreeCtrlBase &tcObject)
      : TNewWellPathBranchObs(entry, tcObject, selection, &TNewWellPathSelection::IsSelected,
                              &TNewWellPathSelection::SetSelected) {
    Update();
  }
};

// Non-meshed surface entry observer class
typedef CSelectionLeafObserver_Delegate<CNonMeshedSurface, CNonMeshedSurface_Delegate, TNonMeshedSurfaceSelection, TRUE>
    TNonMeshedSurfaceObs;
typedef CSelectionBranchObserver_Delegate<CNonMeshedSurfaceEntry, CNonMeshedSurfaceEntry_Delegate, CNonMeshedSurface,
                                          TNonMeshedSurfaceObs, CNonMeshedSurface, TNonMeshedSurfaceSelection, FALSE,
                                          FIXED_ITEM>
    TNonMeshedSurfaceBranchObs;

class CNonMeshedSurfaceEntryObserver : public TNonMeshedSurfaceBranchObs {
public:
  CNonMeshedSurfaceEntryObserver(CNonMeshedSurfaceEntry &entry, TNonMeshedSurfaceSelection &selection,
                                 CTreeCtrlBase &tcObject)
      : TNonMeshedSurfaceBranchObs(entry, tcObject, selection, &TNonMeshedSurfaceSelection::IsSelected,
                                   &TNonMeshedSurfaceSelection::SetSelected) {
    Update();
  }
};

template <class ENTRY_OBSERVER, class ENTRY, unsigned int ENTRY_ID, class EXPORT_SELECTION>
ENTRY_OBSERVER *CreateExportResultTreeBranch(CModelBase &model, EXPORT_SELECTION &selection, CTreeCtrlBase &tree) {
  ENTRY *pEntry = (ENTRY *)model.GraphEntry(ENTRY_ID);
  ENTRY_OBSERVER *entryObserver = 0;

  if (pEntry)
    entryObserver = new ENTRY_OBSERVER(*pEntry, selection, tree);

  return entryObserver;
}

} // namespace re

#endif // _EXPORTRESULTTREEBRANCH_H_
