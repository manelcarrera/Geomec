#include "stdafx.h"

#include "HotSpot_Delegate.h"
#include "HotSpot.h"
#include "AttriHotSpotDlg.h"

CHotSpot_Delegate::CHotSpot_Delegate(CHotSpot* hotSpot)
: IColorScaleNode_Delegate(hotSpot)
, m_hotSpot(hotSpot)
{
}

bool CHotSpot_Delegate::Attributes()
{
  CAttriHotSpotDlg dlg(*m_hotSpot);

  return dlg.DoModal() == IDOK;
}