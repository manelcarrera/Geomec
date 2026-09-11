#ifndef NEWWELLPATHOBSERVER_H
#define NEWWELLPATHOBSERVER_H

#include "GraphPtr.h"
#include "GroupNode.h"
#include "IPolyLine.h"
#include "LabelPoint.h"
#include "NewWellPathBase.h"
#include "NewWellPoint.h"
#include "NewWellSection.h"
#include "NodeObserver.h"
#include "OpenGLNodeObserver.h"
#include "OpenGLNodeObserver_Delegate.h"
#include "VectorSymbol.h"
#include "colornode.h"
#include "openglnode.h"
#include <list>

class CFemAppModel;
class C3DModel;
class CWellZoomInModel;
class CModelBase;
class geo::IMesh;
class geo::IFace;
class CFormationBase;
class CNewWellPath;

#include "FormationBase.h"
#include "WellZoomInModel_Delegate.h"

//*******************************************************************
//
//	CNewWellPathObserver
//
//*******************************************************************
typedef CNodeObserver_Delegate<CWellZoomInModel, CWellZoomInModel_Delegate, CDummyNode, CDummyObserver, FALSE,
                               FIXED_ITEM>
    TNewZoomInObs;
class CNewWellPathObserver : public COpenGLNodeObserver_Delegate<CNewWellPath, CNewWellPath_Delegate, CWellZoomInModel,
                                                                 TNewZoomInObs, FALSE, FIXED_ITEM> {
public:
  CNewWellPathObserver(CNewWellPath &node, CTreeNode &parent, const BOOL rename, const enum REMOVE_TYPE remove,
                       HTREEITEM hInsertAfter = TVI_LAST, BOOL bInitialUpdate = TRUE, BOOL bSortAfterInsertion = FALSE)
      : COpenGLNodeObserver_Delegate<CNewWellPath, CNewWellPath_Delegate, CWellZoomInModel, TNewZoomInObs, FALSE,
                                     FIXED_ITEM>(node, parent, rename, remove, hInsertAfter, bInitialUpdate,
                                                 bSortAfterInsertion) {}

  CNewWellPathObserver(CNewWellPath &node, CTreeCtrl &ctrl, const BOOL rename, const enum REMOVE_TYPE remove,
                       HTREEITEM hParent = TVI_ROOT, HTREEITEM hInsertAfter = TVI_LAST, BOOL bInitialUpdate = TRUE,
                       BOOL bSortAfterInsertion = FALSE)
      : COpenGLNodeObserver_Delegate<CNewWellPath, CNewWellPath_Delegate, CWellZoomInModel, TNewZoomInObs, FALSE,
                                     FIXED_ITEM>(node, ctrl, rename, remove, hParent, hInsertAfter, bInitialUpdate,
                                                 bSortAfterInsertion) {}

  virtual ~CNewWellPathObserver() {}

  virtual BOOL OnBeginDrag() {
    CGroupNode *group = static_cast<CNewWellPath &>(ObservedItem()).GetGroup();
    return group && group->ReadOnly() ? FALSE : TRUE;
  }
};

#endif // NEWWELLPATHOBSERVER_H
