#include "stdafx.h"

#include "AttriHotSpotDlg.h"
#include "HotSpot.h"
#include "HotSpot_Delegate.h"

CHotSpot_Delegate::CHotSpot_Delegate(CHotSpot *hotSpot) : IColorScaleNode_Delegate(hotSpot), m_hotSpot(hotSpot) {}

bool CHotSpot_Delegate::Attributes() {
  CAttriHotSpotDlg dlg(*m_hotSpot);

  return dlg.DoModal() == IDOK;
}