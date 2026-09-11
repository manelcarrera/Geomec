#include "stdafx.h"

#include "OIV2DGraphValueColorScene.h"

OIV2DGraphValueColorScene::OIV2DGraphValueColorScene(CFemAppModel &model) : OIV2DScene(model) {}

OIV2DGraphValueColorScene::OIV2DGraphValueColorScene(const QString &name, CFemAppModel &model)
    : OIV2DScene(name, model) {}

OIV2DGraphValueColorScene::OIV2DGraphValueColorScene(unsigned int name, CFemAppModel &model)
    : OIV2DScene(name, model) {}

void OIV2DGraphValueColorScene::SaveStream(TSTREAM &stream, TPROGRESS &progress) {
  OIV2DScene::SaveStream(stream, progress);
}

void OIV2DGraphValueColorScene::LoadStream(TSTREAM &stream, CStreamVersion &version, TPROGRESS &progress) {
  OIV2DScene::LoadStream(stream, version, progress);
}
