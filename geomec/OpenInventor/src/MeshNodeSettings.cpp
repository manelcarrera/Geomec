#include "stdafx.h"

#include "MeshNodeSettings.h"

MeshNodeSettings::MeshNodeSettings()
    : contourLineModeSelected(false), meshLinesDisplayed(true), formationOutlinesDisplayed(false),
      showValuesPerElement(false), viewBeachBalls(true), meshMode(SkinMesh), tensorVectorMode(TensorVectorFull),
      meshFilled(true), transparency(0), isoCount(10), isoGap(0), vectorScale(1), vectorBaseScale(1),
      beachBallBaseScale(1.0f), vectorVisibleFraction(1) {}
