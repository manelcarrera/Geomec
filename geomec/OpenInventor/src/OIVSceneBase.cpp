#include "OIVSceneBase.h"
#include "GeomecDoc.h"
#include "ModelView.h"
#include "stdafx.h"
#include <cassert>

COIVSceneBase::COIVSceneBase(CFemAppModel &model) : CStorageNode(model) {}

COIVSceneBase::COIVSceneBase(const QString &strName, CFemAppModel &model) : CStorageNode(strName, model) {}

COIVSceneBase::COIVSceneBase(unsigned int uName, CFemAppModel &model) : CStorageNode(uName, model) {}

COIVSceneBase::~COIVSceneBase() {}

void COIVSceneBase::Activate() {}

void COIVSceneBase::Deactivate() {}

CLegendFrame &COIVSceneBase::LegendFrame() {
  CModelView *pView = GetGeomecDoc()->GetModelView();
  assert(pView);
  assert(pView->LegendView());
  assert(pView->LegendView()->ChildSize() == 1);
  CLegendFrame *pFrame = dynamic_cast<CLegendFrame *>(&pView->LegendView()->Child(0));
  assert(pFrame);
  return *pFrame;
}
