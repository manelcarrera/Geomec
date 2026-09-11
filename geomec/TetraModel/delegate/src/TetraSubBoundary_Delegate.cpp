#include "stdafx.h"

#include "TetraSubBoundary_Delegate.h"

CTetraSubBoundary_Delegate::CTetraSubBoundary_Delegate(
  CTetraSubBoundary* tetraSubBoundary)
: CColorNode_Delegate(tetraSubBoundary)
, m_tetraSubBoundary(tetraSubBoundary)
{
}

void CTetraSubBoundary_Delegate::AppendContextMenu(
  CContextMenuInvoker &invoker)
{
}

CTetraSubBoundary_Delegate::CHorizonPlaceHolder_Delegate::
  CHorizonPlaceHolder_Delegate(
  CTetraSubBoundary::CHorizonPlaceHolder* horizonPlaceHolder)
: COpenGLNode_Delegate(horizonPlaceHolder)
, m_horizonPlaceHolder(horizonPlaceHolder)
{
}

void CTetraSubBoundary_Delegate::CHorizonPlaceHolder_Delegate::
  AppendContextMenu(CContextMenuInvoker &invoker)
{
}
