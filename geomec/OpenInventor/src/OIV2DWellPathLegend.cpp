#include "stdafx.h"

#include "NewWellPath.h"
#include "OIV2DWellPathLegend.h"

OIV2DWellPathLegend::OIV2DWellPathLegend() : OIV2DLegend() {
  addLegendSection();
  addLegendSection();
  addLegendSection();
  addLegendSection();
  addLegendSection();
}

void OIV2DWellPathLegend::setupFormationSections(const CNewWellPath &wellPath) {
  addLeader(4, "Formations");
  for (auto section : wellPath.FormationSections()) {
    QColor color = section.Formation()->Color();
    QString name = section.Formation()->Name();

    addItem(4, name.toStdString(), color.redF(), color.greenF(), color.blueF());
  }
}
