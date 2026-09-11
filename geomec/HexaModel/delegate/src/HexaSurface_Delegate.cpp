#include "stdafx.h"

#include "ContextMenuInvoker.h"
#include "FemAppModel.h"
#include "HexaEntryTypes.h"
#include "HexaHorizon.h"
#include "HexaSurface.h"
#include "HexaSurface_Delegate.h"

CHexaSurface_Delegate::CHexaSurface_Delegate(CHexaSurface *hexaSurface)
    : CSurfaceBase_Delegate(hexaSurface), m_hexaSurface(hexaSurface) {}

void CHexaSurface_Delegate::AppendContextMenu(CContextMenuInvoker &invoker) {
  CHexaHorizonEntry *pEntry = (CHexaHorizonEntry *)(m_hexaSurface->Model().GraphEntry(MD_HEXA_HORIZON));

  if (pEntry->CanConnectItem(*m_hexaSurface)) {
    invoker.AddCommand(_T("Create horizon"),
                       *(new CHexaSurface::THexaSurfaceCommand(*m_hexaSurface, &CHexaSurface::CreateHorizon)));
    invoker.AddSeparator();
  }

  CSurfaceBase_Delegate::AppendContextMenu(invoker);
}
