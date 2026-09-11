// TetraFormationObserver.cpp: implementation of the CTetraFormation class.
//
//////////////////////////////////////////////////////////////////////

#include "CalculatedTemperature.h"
#include "FractureMatrixPressure.h"
#include "GeomecDianaRunner.h"
#include "GocadImport.h"
#include "MaterialServer.h"
#include "MeshBase.h"
#include "Pressure.h"
#include "StrainLoad.h"
#include "SurfaceDesc.h"
#include "Temperature.h"
#include "TetSurface.h"
#include "TetraEntryTypes.h"
#include "TetraFormation.h"
#include "TetraHorizonBase.h"
#include "TetraMesh.h"
#include "TetraModel.h"
#include "TnoFileDialog.h"
#include "VolumetricStrainLoad.h"
#include "geomec.h"
#include "stdafx.h"

#include <map>

#include "EnumerationBranch.h"
#include "ValueCompositeObserver.h"

#ifdef _DEBUG
#ifdef _MSC_VER
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif // _MSC_VER
// #define new DEBUG_NEW
#endif

#include "MaterialServerObserver_Delegate.h"
#include "MaterialServer_Delegate.h"
#include "TetraFormationObserver.h"

typedef COpenGLNodeObserver_Delegate<IFormationElementSet, IFormationElementSet_Delegate, CDummyNode, CDummyObserver,
                                     FALSE, FIXED_ITEM>
    TElementSetObserver;
typedef CEnumerationBranch<IFormationElementSet, TElementSetObserver, TRUE, DELETE_ITEM> TElementSetEnumerator;

CTetraFormationObserver::CTetraFormationObserver(CTetraFormation &formation, CTreeNode &parent, const BOOL rename,
                                                 const REMOVE_TYPE remove)
    : TTetraFormationObserver(formation, parent, FALSE, remove) {}

CTetraFormationObserver::TChildren CTetraFormationObserver::ElementSetObservers() const {
  TChildren vcGrandChildren;

  /* The lines below solved Bug 72323, but caused Bug 82643; since the latter is far more annoying, we're going to look
  for another solution to 72323 if(!IsExpanded())
  {
  (const_cast<CTreeCtrl&>(Ctrl())).Expand(Handle(), TVE_EXPAND);
  (const_cast<CTreeCtrl&>(Ctrl())).Expand(Handle(), TVE_COLLAPSE);
  }
  */

  // only consider formation elementset children
  TChildren vcChildren = Children();

  for (size_t i = 0; i < vcChildren.size(); ++i) {
    TElementSetEnumerator *pChild = dynamic_cast<TElementSetEnumerator *>(vcChildren[i]);
    if (pChild) {
      vcGrandChildren = pChild->Children();
      break; // only one
    }
  }

  return vcGrandChildren;
}

BOOL CTetraFormationObserver::AllSameState() const {
  unsigned int uState = 0;
  TChildren children = ElementSetObservers();

  for (TChildren::iterator it = children.begin(); it != children.end(); it++) {
    if ((*it)->StateIcon() != 0 && (*it)->StateIcon() != uState) {
      if (uState == 0) {
        uState = (*it)->StateIcon();
      } else {
        return FALSE;
      }
    }
  }

  return TRUE;
}

unsigned int CTetraFormationObserver::StateIcon() const {
  return TTetraFormationObserver::StateIcon();
#if 0
  CGeomecDoc* pDoc = &((CGeomecApp*) AfxGetApp())->GetDoc();
  unsigned int uiFirst = (pDoc->CurrentScene() && pDoc->CurrentScene()->SelectMode() == COpenGLSceneNode::SM_CHECK ? IDI_UNCHECKED : 0);

  TChildren vcGrandChildren = ElementSetObservers();

  for(size_t j = 0; j < vcGrandChildren.size(); ++j)
  {
  assert(dynamic_cast<TElementSetObserver*>(vcGrandChildren[j]));
  TElementSetObserver& obs = static_cast<TElementSetObserver&>(*vcGrandChildren[j]);
  if(!j)
  {
      uiFirst = obs.StateIcon();
  }
  else if(obs.StateIcon() != uiFirst)
  {
      uiFirst = IDI_TRI_CHECKED;
      break;
  }
  }

  return uiFirst;
#endif
}

void CTetraFormationObserver::ToggleState() {
  TTetraFormationObserver::ToggleState();
  return;
#if 0
  TChildren vcGrandChildren = ElementSetObservers();

  if(!vcGrandChildren.empty())
  {
  if(AllSameState())
  {
      // toggle all
      for(size_t i = 0; i < vcGrandChildren.size(); ++i)
      {
    vcGrandChildren[i]->ToggleState();
    vcGrandChildren[i]->Update();
      }
  }
  else
  {
      unsigned int uTarget = vcGrandChildren[0]->StateIcon();
      for(size_t i = 1; i < vcGrandChildren.size(); ++i)
      {
    if(vcGrandChildren[i]->StateIcon() != uTarget)
    {
          vcGrandChildren[i]->ToggleState();
          vcGrandChildren[i]->Update();
    }
      }
  }
  }
#endif
}

//=============================================================================

CTetraFormationEntryObserver::CTetraFormationEntryObserver(CTetraFormationEntry &entry, CTreeCtrl &ctrl,
                                                           HTREEITEM hParent, HTREEITEM hInsertAfter)
    : CStateBranch_Delegate<CTetraFormationEntry, CTetraFormationEntry_Delegate, CTetraFormation,
                            CTetraFormationObserver, CUndefinedIconProvider, TRUE, DELETE_ITEM>(
          entry, ctrl, FALSE, FIXED_ITEM, hParent, hInsertAfter, FALSE, FALSE) {
  Update();
}

CString CTetraFormationEntryObserver::FormationElementSetTitle() const { return CString(_T("Bodies")); }

unsigned int CTetraFormationEntryObserver::FormationElementSetIcon() const { return IDI_BODY; }

CTreeNode *CTetraFormationEntryObserver::InsertChild(CTetraFormation &t) {
  CTreeNode *pTreeObserver =
      CStateBranch_Delegate<CTetraFormationEntry, CTetraFormationEntry_Delegate, CTetraFormation,
                            CTetraFormationObserver, CUndefinedIconProvider, TRUE, DELETE_ITEM>::InsertChild(t);

#if 1
  typedef CEnumerationBranch<CPressure, CValueCompositeObserver, FALSE, FIXED_ITEM> TPressureEnumerator;
  typedef CEnumerationBranch<CFractureMatrixPressure, CValueCompositeObserver, FALSE, FIXED_ITEM>
      TFractureMatrixPressureEnumerator;
  typedef CEnumerationBranch<CTemperature, CValueCompositeObserver, FALSE, FIXED_ITEM> TTemperatureEnumerator;
  typedef CEnumerationBranch<CCalculatedTemperature, CValueCompositeObserver, FALSE, FIXED_ITEM> TCalcTemperEnumerator;
  typedef CEnumerationBranch<CStrainLoad, CValueCompositeObserver, FALSE, FIXED_ITEM> TStrainEnumerator;
  typedef CMaterialServerObserver_Delegate<CMaterialServer, CMaterialServer_Delegate, CValueType,
                                           CDistributedMaterialPropertyObserver, FALSE, UNLINK_ITEM>
      TMaterialObserver;
  typedef CEnumerationBranch<CMaterialServer, TMaterialObserver, TRUE, FIXED_ITEM> TMaterialEnumerator;

  new TElementSetEnumerator(*pTreeObserver, FormationElementSetTitle(), FormationElementSetIcon());

  new TPressureEnumerator(*pTreeObserver, _T("Pressure"), IDI_PRESSURES);

  new TFractureMatrixPressureEnumerator(*pTreeObserver, _T("Fracture matrix pressure"), IDI_PRESSURES);

  new TTemperatureEnumerator(*pTreeObserver, _T("Temperature"), IDI_TEMPERATURES);

  new TCalcTemperEnumerator(*pTreeObserver, _T("Calculated temperatures"), IDI_CALC_TEMPERS);

  new TStrainEnumerator(*pTreeObserver, _T("Strains"), IDI_VOLUMETRIC_STRAINS);

  new TMaterialEnumerator(*pTreeObserver, _T("Materials"), IDI_ROCK_MATERIALS);

  new CValueCompositeObserver(t.FormationThickness(), *pTreeObserver, FALSE, FIXED_ITEM, TVI_LAST, FALSE, FALSE);
#endif

  return pTreeObserver;
}
