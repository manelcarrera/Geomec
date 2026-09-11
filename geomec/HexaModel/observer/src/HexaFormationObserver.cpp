// HexaFormationObserver.cpp: implementation of the CHexaFormation class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "Geomec.h"
#include "HexaEntryTypes.h"
#include "HexaFormation.h"
#include "HexaHorizon.h"
#include "Pressure.h"
#include "FractureMatrixPressure.h"
#include "OpenGLNodeObserver.H"
#include "MaterialServer.h"
#include "ValueCompositeObserver.h"
#include "HexaModel.h"
#include "FormationPlane.h"
#include "CrossSection.h"
#include "GlobalInitialStressNode.h"
#include "Pressure.h"
#include "FormationVolume.h"
#include "BranchState.h"
#include "Temperature.h"
#include "CalculatedTemperature.h"
#include "VolumetricStrainLoad.h"
#include "MainFrm.h"
#include "StrainLoad.h"

/* From IFormationElementSet constructor */
#include "MeshBase.h"
/* ************************************* */

#include "NodeObserver_Delegate.h"
#include "HexaHorizon_Delegate.h"
#include "MaterialServer_Delegate.h"
#include "MaterialServerObserver_Delegate.h"
#include "HexaFormationObserver.h"
#include "StateEnumerationBranch.h"

CHexaFormationEntryObserver::CFormationPlaneObs::CFormationPlaneObs(CFormationPlane& node,
                                  CTreeNode &parent,
                                  const BOOL rename,
                                  const enum REMOVE_TYPE remove,
                                  HTREEITEM hInsertAfter,
                                  BOOL bInitialUpdate,
                                  BOOL bSortAfterInsertion)
: TFormationPlaneObs(node, parent, rename, remove, hInsertAfter, bInitialUpdate, bSortAfterInsertion)
{
}

CHexaFormationEntryObserver::CFormationPlaneObs::CFormationPlaneObs(CFormationPlane& node,
                                  CTreeCtrl &ctrl,
                                  const BOOL rename,
                                  const enum REMOVE_TYPE remove,
                                  HTREEITEM hParent,
                                  HTREEITEM hInsertAfter,
                                  BOOL bInitialUpdate,
                                  BOOL bSortAfterInsertion)
: TFormationPlaneObs(node, ctrl, rename, remove, hParent, hInsertAfter, bInitialUpdate, bSortAfterInsertion)
{
}

unsigned int CHexaFormationEntryObserver::CFormationPlaneObs::StateIcon() const
{
  return TFormationPlaneObs::StateIcon();
}

void CHexaFormationEntryObserver::CFormationPlaneObs::ToggleState()
{
  TFormationPlaneObs::ToggleState();
}



CHexaFormationEntryObserver::CHexaFormationEntryObserver(CHexaFormationEntry &entry,
                             CTreeCtrl &ctrl,
                             HTREEITEM hParent,
                             HTREEITEM hInsertAfter)
: CStateBranch_Delegate<CHexaFormationEntry, CHexaFormationEntry_Delegate, CHexaFormation, THexaFormationObserver, CUndefinedIconProvider, TRUE, FIXED_ITEM>(entry, ctrl, FALSE, FIXED_ITEM, hParent, hInsertAfter, FALSE, FALSE) 
{
  Update();
}

CTreeNode* CHexaFormationEntryObserver::InsertChild(CHexaFormation &t)
{
  typedef CEnumerationBranch<CPressure, CValueCompositeObserver, FALSE, FIXED_ITEM> TPressureEnumerator;
  typedef CEnumerationBranch<CFractureMatrixPressure, CValueCompositeObserver, FALSE, FIXED_ITEM> TFractureMatrixPressureEnumerator;
  typedef CEnumerationBranch<CTemperature, CValueCompositeObserver, FALSE, FIXED_ITEM> TTemperatureEnumerator;
  typedef CEnumerationBranch<CCalculatedTemperature, CValueCompositeObserver, FALSE, FIXED_ITEM> TCalcTemperEnumerator;
  typedef CEnumerationBranch<CStrainLoad, CValueCompositeObserver, FALSE, FIXED_ITEM> TStrainEnumerator;
  typedef COpenGLNodeObserver_Delegate <CHexaHorizon, CHexaHorizon_Delegate, CDummyNode, CDummyObserver, FALSE, FIXED_ITEM> THorizonObj;
  typedef CEnumerationBranch<CHexaHorizon, THorizonObj, TRUE, FIXED_ITEM> THorizonEnumerator;
  typedef CEnumerationBranch<CFormationPlane, CFormationPlaneObs, TRUE, DELETE_ITEM> TFormationPlaneEnumerator;
  typedef CMaterialServerObserver_Delegate<CMaterialServer, CMaterialServer_Delegate, CValueType, CDistributedMaterialPropertyObserver, FALSE, UNLINK_ITEM> TMaterialObserver;
  typedef CStateEnumerationBranch<CMaterialServer, TMaterialObserver, TRUE, UNLINK_ITEM, CUndefinedIconProvider> TMaterialEnumerator;

  CTreeNode *pTreeObserver = CStateBranch_Delegate<CHexaFormationEntry, CHexaFormationEntry_Delegate, CHexaFormation, THexaFormationObserver, CUndefinedIconProvider, TRUE, FIXED_ITEM>::InsertChild(t);

  new THorizonEnumerator(*pTreeObserver,
              _T("Horizons"),
              IDI_HORIZONS);
#if 1
  new TPressureEnumerator(*pTreeObserver,
               _T("Pressure"),
               IDI_PRESSURES);

  new TFractureMatrixPressureEnumerator(*pTreeObserver,
               _T("Fracture matrix pressure"),
               IDI_PRESSURES);

  new TTemperatureEnumerator(*pTreeObserver,
                 _T("Temperature"),
                 IDI_TEMPERATURES);

  new TCalcTemperEnumerator(*pTreeObserver,
                 _T("Calculated temperatures"),
                 IDI_CALC_TEMPERS);

  new TStrainEnumerator(*pTreeObserver,
                 _T("Strains"),
                 IDI_VOLUMETRIC_STRAINS);

  new TFormationPlaneEnumerator(*pTreeObserver,
               _T("Planes"),
               IDI_FORMATIONPLANES);
/*
  new TMaterialObserver(t.Material(),
              *pTreeObserver,
              TRUE,
              DELETE_ITEM,
              TVI_LAST,
              TRUE,
              FALSE);
*/
  new TMaterialEnumerator(*pTreeObserver,
                          _T("Materials"),
                          IDI_ROCK_MATERIALS);

  new CValueCompositeObserver(t.FormationThickness(),
                              *pTreeObserver,
                              FALSE,
                              FIXED_ITEM,
                              TVI_LAST,
                              FALSE,
                              FALSE);
#endif
  return pTreeObserver;
}
