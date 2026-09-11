#ifndef _WELLCASINGCEMENTOBSERVER_H_
#define _WELLCASINGCEMENTOBSERVER_H_

#include "MaterialServerParent.h"
#include "IRockMaterialServerTempl.h"
#include "OpenGLNodeObserver.h"
#include "FormationBase.h"

class CWellCasingModel;
class CWellCasingCementInitialMaterial;

#include "WellCasingCement_Delegate.h"
#include "OpenGLNodeObserver_Delegate.h"

typedef COpenGLNodeObserver_Delegate<CWellCasingCement, CWellCasingCement_Delegate, CDummyNode, CDummyObserver, FALSE, FIXED_ITEM> TWellCasingCementObserver;
class CWellCasingCementObserver : public TWellCasingCementObserver
{
public:
  CWellCasingCementObserver(CWellCasingCement& node,
              CTreeCtrl& ctrl,
              const BOOL rename,
              const enum REMOVE_TYPE remove,
              HTREEITEM hParent = TVI_ROOT,
              HTREEITEM hInsertAfter = TVI_LAST,
              BOOL bInitialUpdate = TRUE,
              BOOL bSortAfterInsertion = TRUE);
};

#endif // _WELLCASINGCEMENTOBSERVER_H_
