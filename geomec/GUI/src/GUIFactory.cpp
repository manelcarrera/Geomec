#include "stdafx.h"

#include "GUIFactory.h"
#include "3DGUI.h"
#include "3dmodel.h"
#include "HexaGUI.h"
#include "HexaModel.h"
#include "TetraGUI.h"
#include "TetraModel.h"
#include "EclipseGUI.h"
#include "EclipseModel.h"
#include "GoCadGUI.h"
#include "GoCadModel.h"
#include "IWellGUI.h"
#include "IWellModel.h"
#include "WellCasingGUI.h"
#include "WellCasingModel.h"
#include "WellZoomInGUI.h"

CGUIFactory::CGUIFactory()
{
}

CGUIBase* CGUIFactory::createGUI(CFemAppGUI* parentGUI,
  CFemAppModel* model)
{
  CGUIBase* GUIBase = 0;

  switch (model->modelType())
  {
  case CFemAppModel::eHexaModel:
      GUIBase = new CHexaGUI(parentGUI,
    dynamic_cast <CHexaModel*> (model));
      break;
  case CFemAppModel::eTetraModel:
      GUIBase = new CTetraGUI(parentGUI,
    dynamic_cast <CTetraModel*> (model));
      break;
  case CFemAppModel::eEclipseModel:
      GUIBase = new CEclipseGUI(parentGUI,
    dynamic_cast <CEclipseModel*> (model));
      break;
  case CFemAppModel::eGoCadModel:
      GUIBase = new CGoCadGUI(parentGUI,
    dynamic_cast <CGoCadModel*> (model));
      break;
  case CFemAppModel::eWellCasingModel:
      GUIBase = new CWellCasingGUI(parentGUI,
    dynamic_cast <CWellCasingModel*> (model));
      break;
  case CFemAppModel::eWellZoomInModel:
      GUIBase = new CWellZoomInGUI(parentGUI,
    dynamic_cast <CWellZoomInModel*> (model));
      break;
  default:
      assert(false);
  }

  assert(GUIBase != 0);

  return GUIBase;
}
