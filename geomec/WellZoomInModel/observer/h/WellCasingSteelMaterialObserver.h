#ifndef _WELLCASINGSTEELMATERIALOBSERVER_H_
#define _WELLCASINGSTEELMATERIALOBSERVER_H_

class CWellCasingSteelMaterialEntry_Delegate;
class CModelBase;

#include "IMaterialBase.h"

class CWellCasingSteelMaterialEntry;

#include "DeletableNodeObserver_Delegate.h"
#include "Geomec.h"
#include "StateBranch_Delegate.h"
#include "WellCasingSteelMaterial_Delegate.h"

typedef CDeletableNodeObserver_Delegate<CWellCasingSteelMaterial, CWellCasingSteelMaterial_Delegate, CDummyNode,
                                        CDummyObserver, FALSE, FIXED_ITEM>
    TWellCasingSteelMaterialObserver;
typedef CStateBranch_Delegate<CWellCasingSteelMaterialEntry, CWellCasingSteelMaterialEntry_Delegate,
                              CWellCasingSteelMaterial, TWellCasingSteelMaterialObserver, CUndefinedIconProvider, TRUE,
                              DELETE_ITEM>
    TWellCasingSteelMaterialEntryObserver;

#endif // _WELLCASINGSTEELMATERIALOBSERVER_H_
