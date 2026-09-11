#include "stdafx.h"

#include "ResultSingleSelectDialog.h"
#include "ValueComponentProxyWrapper.h"

bool valueComponentProxyWrapper(rpn::CRpnOperand::IValueProxy &proxy) {
  CDerivedResult::CDerivedResultComponent::CValueComponentProxy &valueComponentProxy =
      dynamic_cast<CDerivedResult::CDerivedResultComponent::CValueComponentProxy &>(proxy);
  CResultSingleSelectDialog dlg(valueComponentProxy, !valueComponentProxy.StoreOnFile());

  if (dlg.DoModal() == IDOK) {
    return valueComponentProxy.Properties(dlg.FixedLinStage(), dlg.Selection(), (LPCSTR)dlg.Name());
  }

  return false;
}
