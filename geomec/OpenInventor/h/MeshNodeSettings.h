#ifndef _MeshNodeSettings_h_
#define _MeshNodeSettings_h_

#include "MeshMode.h"

struct MeshNodeSettings
{
  bool                contourLineModeSelected;
  bool                meshLinesDisplayed;
  bool                formationOutlinesDisplayed;
  bool                showValuesPerElement;
  bool                viewBeachBalls;
  MeshMode            meshMode;
  TensorVectorMode    tensorVectorMode;
  bool                meshFilled;
  int                 isoCount;
  double              isoGap;
  double              dataMin;
  double              dataMax;
  int                 transparency;
  
  // This is a relative scaling factor that the user can set with the slider in the
  // toolbar. The actual scale that is used is the product of vectorBaseScale
  float               vectorScale;
  // Base scale is the scaling factor that OIDIMeshNodeManager computes,
  // in order to make the size of the largest vector be some percentage
  // of the bounding box size
  float               vectorBaseScale;
  float               beachBallBaseScale;
  float               vectorVisibleFraction;

  MeshNodeSettings();
};

#endif  // _MeshNodeSettings_h_
