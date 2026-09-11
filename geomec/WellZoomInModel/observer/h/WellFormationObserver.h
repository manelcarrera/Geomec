#ifndef _WELLZOOMINFORMATIONOBSERVER_H_
#define _WELLZOOMINFORMATIONOBSERVER_H_

#include "3DFormation.h"

class IWellModel;
class IMaterialRock;
class CWellFormationEntry;

#include "WellFormation_Delegate.h"
#include "StateBranch_Delegate.h"
#include "OpenGLNodeObserver_Delegate.h"
#include "UndefinedIconProvider.h"

typedef COpenGLNodeObserver_Delegate<CWellFormation, CWellFormation_Delegate, CDummyNode, CDummyObserver, FALSE, FIXED_ITEM> TWellFormationObserver;

class CWellFormationEntryObserver : public CStateBranch_Delegate<CWellFormationEntry, CWellFormationEntry_Delegate, CWellFormation, TWellFormationObserver, CUndefinedIconProvider, TRUE, FIXED_ITEM>
{
public:
  CWellFormationEntryObserver(CWellFormationEntry& entry, CTreeCtrl& ctrl, HTREEITEM hParent = TVI_ROOT, HTREEITEM hInsertAfter = TVI_LAST);

  CTreeNode* InsertChild(CWellFormation& t);
};

#endif // _WELLZOOMINFORMATIONOBSERVER_H_
