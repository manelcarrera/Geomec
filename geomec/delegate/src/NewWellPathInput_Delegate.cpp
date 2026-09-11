#include "stdafx.h"

#include "NewWellPathInput_Delegate.h"
#include "NewWellPathInput.h"
#include "ContextMenuInvoker.h"


CNewWellPathInput_Delegate::CNewWellPathInput_Delegate(CNewWellPathInput* wellpathInput)
  : CPointSet_Delegate(wellpathInput)
  , m_wellpathInput(wellpathInput)
{
}

void CNewWellPathInput_Delegate::AppendContextMenu(CContextMenuInvoker& invoker)
{
  invoker.AddCommand(_T("&Flip Depth"), *(new CSingleCommandTemplate<CNewWellPathInput_Delegate>(*this, &CNewWellPathInput_Delegate::FlipDepth)));
  invoker.AddSeparator();

  CPointSet_Delegate::AppendContextMenu(invoker);
}

void CNewWellPathInput_Delegate::FlipDepth()
{
  m_wellpathInput->FlipDepth();
}
