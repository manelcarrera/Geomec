// GraphTreeCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "GraphNode.h"
#include "GraphPtr.h"
#include "GraphEntry.h"
#include "FemAppModel.h"
#include "graphtreectrl.h"
#include "TreeNode.h"
#include "FemAppDoc.h"
#include "GraphMainFrm.h"
#include "GraphTreeView.h"
#include "FemAppMainWindow.h"

#ifdef _DEBUG
//#define new DEBUG_NEW
#ifdef _MSC_VER
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif  // _MSC_VER
#endif

/////////////////////////////////////////////////////////////////////////////
// CGraphTreeCtrl

CGraphTreeCtrl::CGraphTreeCtrl(CGraphTreeView &view)
: m_view(view)
{
}


BEGIN_MESSAGE_MAP(CGraphTreeCtrl, CTreeCtrlBase)
  //{{AFX_MSG_MAP(CTreeCtrlBase)
  ON_NOTIFY_REFLECT(TVN_GETDISPINFO, OnGetDisplayInfo)
  ON_NOTIFY_REFLECT(TVN_BEGINLABELEDIT, OnBeginLabelEdit)
  ON_NOTIFY_REFLECT(TVN_ENDLABELEDIT, OnEndLabelEdit)
  ON_NOTIFY_REFLECT(TVN_SELCHANGED, OnSelChanged)
  ON_WM_CONTEXTMENU()
  ON_NOTIFY_REFLECT(TVN_BEGINDRAG, OnBeginDrag)
  ON_WM_LBUTTONUP()
  ON_WM_CREATE()
  ON_WM_LBUTTONDOWN()
  ON_NOTIFY_REFLECT(NM_RCLICK, OnRightClick)
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTreeCtrlBase message handlers



void CGraphTreeCtrl::OnSelect(ITreeObject* pOld, ITreeObject* pNew)
{
  if(pNew)
    m_view.OnSelect(*pNew);
}

void CGraphTreeCtrl::OnContext(const TTreeObjectVec &vcContext)
{
  if(vcContext.size() == 1)
    m_view.Document()->OnContextMenu(*vcContext[0]);

}

CWnd& CGraphTreeCtrl::GetContextWnd() const
{
  return *FemAppGetMainWnd();
}

CGraphTreeView& CGraphTreeCtrl::TreeView()
{
  return m_view;
}

const CGraphTreeView& CGraphTreeCtrl::TreeView() const
{
  return m_view;
}