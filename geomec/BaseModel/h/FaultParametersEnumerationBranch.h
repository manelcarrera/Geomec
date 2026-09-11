#ifndef _FaultParametersEnumerationBranch_h_
#define _FaultParametersEnumerationBranch_h_

#include "FaultParametersHub.h"
#include "ContextMenuInvoker.h"

template <class CHILD_TYPE, class CHILD_OBS_TYPE, BOOL RENAME,
  enum REMOVE_TYPE REMOVE> class CFaultParametersEnumerationBranch :
  public CEnumerationBranch <CHILD_TYPE, CHILD_OBS_TYPE, RENAME, REMOVE>
{
  public:
  CFaultParametersEnumerationBranch(CTreeNode& parent_node,
      const CString& strName, const unsigned int uIcon,
      HTREEITEM hInsertAfter = TVI_LAST, const BOOL bInitialUpdate = TRUE);

  virtual void AppendContextMenu(CContextMenuInvoker& invoker);

  private:
  CFaultParametersEnumerationBranch(
      const CFaultParametersEnumerationBranch& rhs);
  CFaultParametersEnumerationBranch& operator = (
      const CFaultParametersEnumerationBranch& rhs);

  CFaultParametersHub m_faultParametersHub;
};

template <class CHILD_TYPE, class CHILD_OBS_TYPE, BOOL RENAME,
  enum REMOVE_TYPE REMOVE>
CFaultParametersEnumerationBranch <CHILD_TYPE, CHILD_OBS_TYPE, RENAME, REMOVE>::
  CFaultParametersEnumerationBranch(CTreeNode& parent_node,
  const CString& strName, const unsigned int uIcon, HTREEITEM hInsertAfter,
  const BOOL bInitialUpdate)
: CEnumerationBranch(parent_node, strName, uIcon, hInsertAfter, bInitialUpdate)
, m_faultParametersHub(
  dynamic_cast <CHorizonBase&> (parent_node.ObservedItem()))
{
}

template <class CHILD_TYPE, class CHILD_OBS_TYPE, BOOL RENAME,
  enum REMOVE_TYPE REMOVE>
void CFaultParametersEnumerationBranch <CHILD_TYPE, CHILD_OBS_TYPE, RENAME,
  REMOVE>::AppendContextMenu(CContextMenuInvoker& invoker)
{
  typedef CSingleCommandTemplate <CFaultParametersHub>
  TFaultParametersHubTemplate;

  invoker.AddCommand(_T("&Attributes"), *(new TFaultParametersHubTemplate(
  m_faultParametersHub, &CFaultParametersHub::attributes,
  &CFaultParametersHub::hasAttributes)));
}

#endif  // _FaultParametersEnumerationBranch_h_
