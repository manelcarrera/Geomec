#ifndef _INTERFACEMATERIALOBSERVER_H_
#define _INTERFACEMATERIALOBSERVER_H_

class CInterfaceMaterialEntry_Delegate;
class CModelBase;
class CInterfaceMaterial_Delegate;

#include "IMaterialBase.h"

class CInterfaceMaterialEntry;

#include "DeletableNodeObserver_Delegate.h"
#include "StateBranch_Delegate.h"
#include "Geomec.h"

typedef CDeletableNodeObserver_Delegate<CInterfaceMaterial, CInterfaceMaterial_Delegate, CDummyNode, CDummyObserver, FALSE, FIXED_ITEM> TInterfaceMaterialObserver;
typedef CStateBranch_Delegate<CInterfaceMaterialEntry, CInterfaceMaterialEntry_Delegate, CInterfaceMaterial, TInterfaceMaterialObserver, CUndefinedIconProvider, TRUE, DELETE_ITEM> TInterfaceMaterialEntryObserver;

#endif // _INTERFACEMATERIALOBSERVER_H_
