// MaterialEntryObserver.h: interface for the CMaterialEntry class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MATERIALENTRYOBSERVER_H__DB22E808_2D7E_4d19_B104_F7249B0B2270__INCLUDED_)
#define AFX_MATERIALENTRYOBSERVER_H__DB22E808_2D7E_4d19_B104_F7249B0B2270__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CMaterialEntry_Delegate;
class IMaterial_Delegate;

#include "IMaterial.h"
// #include "MatLib.h"
#include "DeletableNodeObserver_Delegate.h"
#include "Geomec.h"
#include "LibraryMaterial.h"
#include "StateBranch_Delegate.h"
#include "mlMaterialLibrary.h"

typedef CDeletableNodeObserver_Delegate<IMaterial, IMaterial_Delegate, CDummyNode, CDummyObserver, FALSE, FIXED_ITEM>
    TMaterialObserver;
typedef CStateBranch_Delegate<CMaterialEntry, CMaterialEntry_Delegate, IMaterial, TMaterialObserver,
                              CUndefinedIconProvider, FALSE, DELETE_ITEM>
    TMaterialEntryObserver;

#endif // !defined(AFX_MATERIALENTRYOBSERVER_H__DB22E808_2D7E_4d19_B104_F7249B0B2270__INCLUDED_)
