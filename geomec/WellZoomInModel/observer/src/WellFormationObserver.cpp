#include "stdafx.h"
#include "geomec.h"

#include "WellFormation.h"
#include "IWellModel.h"
#include "WellModelEntryTypes.h"
#include "ValueCompositeObserver.h"
#include "ValueType.h"
#include "BaseEntryTypes.h"
#include "MaterialServer.h"
#include "MaterialEntry.h"
#include "Pressure.h"
#include "FractureMatrixPressure.h"
#include "Temperature.h"
#include "CalculatedTemperature.h"
#include "VolumetricStrainLoad.h"
#include "MaterialServer_Delegate.h"
#include "MaterialServerObserver_Delegate.h"
#include "EnumerationBranch.h"
#include "WellFormationObserver.h"
#include "StrainLoad.h"

/////

CWellFormationEntryObserver::CWellFormationEntryObserver(CWellFormationEntry& entry, CTreeCtrl& ctrl, HTREEITEM hParent, HTREEITEM hInsertAfter)
: CStateBranch_Delegate<CWellFormationEntry, CWellFormationEntry_Delegate, CWellFormation, TWellFormationObserver, CUndefinedIconProvider, TRUE, FIXED_ITEM>(entry, ctrl, FALSE, FIXED_ITEM, hParent, hInsertAfter, FALSE, FALSE)
{
  Update();
}

CTreeNode* CWellFormationEntryObserver::InsertChild(CWellFormation& t)
{
	typedef CEnumerationBranch<CPressure, CValueCompositeObserver, FALSE, FIXED_ITEM> TPressureEnumerator;
	typedef CEnumerationBranch<CFractureMatrixPressure, CValueCompositeObserver, FALSE, FIXED_ITEM> TFractureMatrixPressureEnumerator;
	typedef CEnumerationBranch<CTemperature, CValueCompositeObserver, FALSE, FIXED_ITEM> TTemperatureEnumerator;
  typedef CEnumerationBranch<CCalculatedTemperature, CValueCompositeObserver, FALSE, FIXED_ITEM> TCalcTemperEnumerator;
  typedef CEnumerationBranch<CStrainLoad, CValueCompositeObserver, FALSE, FIXED_ITEM> TStrainEnumerator;
  typedef CMaterialServerObserver_Delegate<CMaterialServer, CMaterialServer_Delegate, CValueType, CDistributedMaterialPropertyObserver, FALSE, UNLINK_ITEM> TMaterialObserver;
  typedef CEnumerationBranch<CMaterialServer, TMaterialObserver, TRUE, FIXED_ITEM> TMaterialEnumerator;

  CTreeNode* pTreeObserver = CStateBranch_Delegate<CWellFormationEntry, CWellFormationEntry_Delegate, CWellFormation, TWellFormationObserver, CUndefinedIconProvider, TRUE, FIXED_ITEM>::InsertChild(t);

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

  return pTreeObserver;
}
