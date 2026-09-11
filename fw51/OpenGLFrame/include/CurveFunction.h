 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */
#ifndef CURVE_FUNCTION_H
#define CURVE_FUNCTION_H

#include "IDrawDef.h"

#include "OpenGLFrameExports.h"

#define CURVE_SEGMENTATION 16

namespace geo {
  class IElement;
};

OPENGLFRAME_EXPORT void setCoordinatesVector1d(double* ctrlpoints, int i_size, const geo::IElement &elem, const double* coef, int k_size);

OPENGLFRAME_EXPORT void setColorVector1d(double* color_ctrlpoint, int i_size, std::vector<IDrawDef::TColor> &vcColor, const double* coef, int k_size);

OPENGLFRAME_EXPORT void setCoordinatesVector2d(double* ctrlpoints, int i_size, int j_size,  const geo::IElement &elem, const double* coef, int k_size);

OPENGLFRAME_EXPORT void setColorVector2d(double* color_ctrlpoint, int i_size, int j_size, std::vector<IDrawDef::TColor> &vcColor, const double* coef, int k_size);

// LINE DEGREE 2

OPENGLFRAME_EXPORT extern const double coef_LineD2[3][3];
OPENGLFRAME_EXPORT extern const double coef_LineD3[4][4];
OPENGLFRAME_EXPORT extern const double coef_TriangleD2[9][6];
OPENGLFRAME_EXPORT extern const double coef_TriangleD3[16][9];
OPENGLFRAME_EXPORT extern const double coef_QuadD2[9][8];
OPENGLFRAME_EXPORT extern const double coef_QuadD3[16][12];
OPENGLFRAME_EXPORT extern const double coef_TriangleD4[25][15];

#endif // CURVE_FUNCTION_H
