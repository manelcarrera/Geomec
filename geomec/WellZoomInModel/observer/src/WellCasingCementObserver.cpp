#include "StdAfx.h"
#include "geomec.h"

#include "WellCasingCement.h"

#include "WellCasingModel.h"
#include "ValueCompositeObserver.h"
#include "Pressure.h"
#include "Temperature.h"
#include "CalculatedTemperature.h"
#include "VolumetricStrainLoad.h"
#include "MaterialServer.h"
#include "WellCasingCementInitialMaterial.h"
#include "MaterialServer_Delegate.h"
#include "MaterialServerObserver_Delegate.h"
#include "EnumerationBranch.h"
#include "WellCasingCementObserver.h"
#include "StrainLoad.h"

///// CWellCasingCementObserver
CWellCasingCementObserver::CWellCasingCementObserver(CWellCasingCement& node,
                                                     CTreeCtrl& ctrl,
                                                     const BOOL rename,
                                                     const enum REMOVE_TYPE remove,
                                                     HTREEITEM hParent,
                                                     HTREEITEM hInsertAfter,
                                                     BOOL bInitialUpdate,
                                                     BOOL bSortAfterInsertion)
: TWellCasingCementObserver(node, ctrl, rename, remove, hParent, hInsertAfter, bInitialUpdate, bSortAfterInsertion)
{
  typedef CEnumerationBranch<CPressure, CValueCompositeObserver, FALSE, FIXED_ITEM> TPressureEnumerator;
  typedef CEnumerationBranch<CTemperature, CValueCompositeObserver, FALSE, FIXED_ITEM> TTemperatureEnumerator;
  typedef CEnumerationBranch<CCalculatedTemperature, CValueCompositeObserver, FALSE, FIXED_ITEM> TCalcTemperEnumerator;
  typedef CEnumerationBranch<CStrainLoad, CValueCompositeObserver, FALSE, FIXED_ITEM> TStrainEnumerator;
  typedef CMaterialServerObserver_Delegate<CMaterialServer, CMaterialServer_Delegate, CValueType, CDistributedMaterialPropertyObserver, FALSE, UNLINK_ITEM> TMaterialObserver;
  typedef CEnumerationBranch<CMaterialServer, TMaterialObserver, TRUE, FIXED_ITEM> TMaterialEnumerator;

  new TPressureEnumerator(*this,
               _T("Pressure"),
               IDI_PRESSURES);

  new TTemperatureEnumerator(*this,
                 _T("Temperature"),
                 IDI_TEMPERATURES);

  new TCalcTemperEnumerator(*this,
                 _T("Calculated temperatures"),
                 IDI_CALC_TEMPERS);

  new TStrainEnumerator(*this,
                 _T("Strains"),
                 IDI_VOLUMETRIC_STRAINS);

  new TMaterialEnumerator(*this,
                          _T("Materials"),
                          IDI_ROCK_MATERIALS);
}