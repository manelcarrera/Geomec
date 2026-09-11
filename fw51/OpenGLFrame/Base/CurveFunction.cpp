 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */
#include "CurveFunction.h"
#include "IElement.h"
#include "IPoint.h"

/*!
  Convert a higher order line to bezier control points.
  \param ctrlpoints The resulting array with coordinates. The dimension needs
                    to be ctrlpoints[i_size][3].
  \param i_size     The number of control points.
  \param elem       The higher order line.
  \param coef       The transformation matrix with dimension [i_size][k_size].
  \param k_size     The number of nodes defining the shape of the element.

  For coef use
  - coef_LineD2 for second order,
  - coef_LineD3 for third order.

  \sa setColorVector1d, setCoordinatesVector2d, setColorVector2d.
  \todo Rework this to a more general matrix vector multiplication.
*/
void setCoordinatesVector1d(double* ctrlpoints, int i_size, const geo::IElement &elem, const double* coef, int k_size )
{
  for (int i=0; i<i_size; i++) {
    ctrlpoints[i*3+0]=0;
    ctrlpoints[i*3+1]=0;
    ctrlpoints[i*3+2]=0;
    for (int k=0; k<k_size; k++) {
      ctrlpoints[i*3+0] += coef[i*k_size+k] * elem.Point(k).X();
      ctrlpoints[i*3+1] += coef[i*k_size+k] * elem.Point(k).Y();
      ctrlpoints[i*3+2] += coef[i*k_size+k] * elem.Point(k).Z();
    }
  }
}

/*!
  Convert color information for a higher order line to bezier equation format.
  \param color_ctrlpoint The resulting array with color values. The dimension
                         needs to be ctrlpoints[i_size][4].
  \param i_size     The number of color values (control points).
  \param vcColor    The color information for the higher order line. One color
                    for each shape-defining node of the element.
  \param coef       The transformation matrix with dimension [i_size][k_size].
  \param k_size     The number of nodes defining the shape of the element, i.e.
                    the number of color entries in vcColor.

  For coef use
  - coef_LineD2 for second order,
  - coef_LineD3 for third order.

  \sa setCoordinatesVector1d, setCoordinatesVector2d, setColorVector2d.
  \todo Rework this to a more general matrix vector multiplication.
*/
void setColorVector1d(double* color_ctrlpoint, int i_size, std::vector<IDrawDef::TColor> &vcColor, const double* coef, int k_size)
{
  for (int i=0; i<i_size; i++) {
    color_ctrlpoint[i*4+0]=0;
    color_ctrlpoint[i*4+1]=0;
    color_ctrlpoint[i*4+2]=0;
    color_ctrlpoint[i*4+3]=0;
    for (int k=0; k<k_size; k++) {
      color_ctrlpoint[i*4+0] += coef[i*k_size+k] * qRed  (vcColor.at(k));
      color_ctrlpoint[i*4+1] += coef[i*k_size+k] * qGreen(vcColor.at(k));
      color_ctrlpoint[i*4+2] += coef[i*k_size+k] * qBlue (vcColor.at(k));
      color_ctrlpoint[i*4+3] += coef[i*k_size+k] * qAlpha(vcColor.at(k));
    }
    color_ctrlpoint[i*4+0] = color_ctrlpoint[i*4+0] / 256.;
    color_ctrlpoint[i*4+1] = color_ctrlpoint[i*4+1] / 256.;
    color_ctrlpoint[i*4+2] = color_ctrlpoint[i*4+2] / 256.;
    color_ctrlpoint[i*4+3] = color_ctrlpoint[i*4+3] / 256.;
  }
}

/*!
  Convert a higher order 2 dimensional shape to a grid of bezier control points.
  \param ctrlpoints The resulting array with coordinates. The dimension needs
                    to be ctrlpoints[i_size][j_size][3].
  \param i_size     The number of control points in first grid direction.
  \param j_size     The number of control points in second grid direction.
  \param elem       The higher order 2 dimensional shape.
  \param coef       The transformation matrix with dimension
                    [i_size*j_size][k_size].
  \param k_size     The number of nodes defining the shape of the element.

  For coef use
  - coef_TriangleD2 for second order triangles,
  - coef_TriangleD3 for third order triangles,
  - coef_QuadD2 for second order quadrilaterals,
  - coef_QuadD3 for third order quadrilaterals.

  \sa setCoordinatesVector1d, setColorVector1d, setColorVector2d.
  \todo Rework this to a more general matrix vector multiplication.
*/
void setCoordinatesVector2d(double* ctrlpoints, int i_size, int j_size,  const geo::IElement &elem, const double* coef, int k_size)
{
  for (int i=0; i<i_size; i++) {
    for (int j=0; j<j_size; j++) {
      ctrlpoints[i*3*j_size+j*3+0]=0;
      ctrlpoints[i*3*j_size+j*3+1]=0;
      ctrlpoints[i*3*j_size+j*3+2]=0;
      for (int k=0; k<k_size; k++) {
        ctrlpoints[i*3*j_size+j*3+0] += coef[(i*j_size+j)*k_size+k] * elem.Point(k).X();
        ctrlpoints[i*3*j_size+j*3+1] += coef[(i*j_size+j)*k_size+k] * elem.Point(k).Y();
        ctrlpoints[i*3*j_size+j*3+2] += coef[(i*j_size+j)*k_size+k] * elem.Point(k).Z();
      }
    }
  }
}

/*!
  Convert color information for a higher order 2 dimensional shape to
  bezier equation format.
  \param color_ctrlpoint The resulting array with color values. The dimension
                         needs to be ctrlpoints[i_size][j_size][4].
  \param i_size     The number of color values (control points) in first grid
                    direction.
  \param j_size     The number of color values (control points) in second grid
                    direction.
  \param vcColor    The color information for the higher order 2 dimensional
                    shape. One color for each shape-defining node of the
		    element.
  \param coef       The transformation matrix with dimension
                    [i_size*j_size][k_size].
  \param k_size     The number of nodes defining the shape of the element, i.e.
                    the number of color entries in vcColor.

  For coef use
  - coef_TriangleD2 for second order triangles,
  - coef_TriangleD3 for third order triangles,
  - coef_QuadD2 for second order quadrilaterals,
  - coef_QuadD3 for third order quadrilaterals.

  \sa setCoordinatesVector1d, setColorVector1d, setCoordinatesVector2d.
  \todo Rework this to a more general matrix vector multiplication.
*/
void setColorVector2d(double* color_ctrlpoint, int i_size, int j_size, std::vector<IDrawDef::TColor> &vcColor, const double* coef, int k_size)
{
  for (int i=0; i<i_size; i++) {
    for (int j=0; j<j_size; j++) {
      color_ctrlpoint[i*4*j_size+j*4+0]=0;
      color_ctrlpoint[i*4*j_size+j*4+1]=0;
      color_ctrlpoint[i*4*j_size+j*4+2]=0;
      color_ctrlpoint[i*4*j_size+j*4+3]=0;
      for (int k=0; k<k_size; k++) {
        color_ctrlpoint[i*4*j_size+j*4+0] += coef[(i*j_size+j)*k_size+k] * qRed  (vcColor.at(k));
        color_ctrlpoint[i*4*j_size+j*4+1] += coef[(i*j_size+j)*k_size+k] * qGreen(vcColor.at(k));
        color_ctrlpoint[i*4*j_size+j*4+2] += coef[(i*j_size+j)*k_size+k] * qBlue (vcColor.at(k));
        color_ctrlpoint[i*4*j_size+j*4+3] += coef[(i*j_size+j)*k_size+k] * qAlpha(vcColor.at(k));
      }
      color_ctrlpoint[i*4*j_size+j*4+0] = color_ctrlpoint[i*4*j_size+j*4+0] / 256.;
      color_ctrlpoint[i*4*j_size+j*4+1] = color_ctrlpoint[i*4*j_size+j*4+1] / 256.;
      color_ctrlpoint[i*4*j_size+j*4+2] = color_ctrlpoint[i*4*j_size+j*4+2] / 256.;
      color_ctrlpoint[i*4*j_size+j*4+3] = color_ctrlpoint[i*4*j_size+j*4+3] / 256.;
    }
  }
}

/*!
  Transformation matrix to transform a Diana shape to the Bezier format.
  Color information can be transformed likewise.

  Applicable to second order line shapes.

  \sa coef_LineD3, setCoordinatesVector1d, setColorVector1d.
*/
const double coef_LineD2 [3][3] = {
//   0    1    2  
  {  1.,  0.,  0.},
  {-0.5,  2.,-0.5},
  {  0.,  0.,  1.}};

/*!
  Transformation matrix to transform a Diana shape to the Bezier format.
  Color information can be transformed likewise.

  Applicable to third order line shapes.

  \sa coef_LineD2, setCoordinatesVector1d, setColorVector1d.
*/
const double coef_LineD3 [4][4] = {
//       0    1    2        3  
  {      1.,  0.,  0.,      0.},
  {-0.83333,  3.,-1.5, 0.33333},
  { 0.33333,-1.5,  3.,-0.83333},
  {      0.,  0.,  0.,      1.}};


/*!
  Transformation matrix to transform a Diana shape to the Bezier format.
  Color information can be transformed likewise.

  Applicable to second order triangular shapes.

  \sa coef_TriangleD3, coef_TriangleD4, setCoordinatesVector2d, setColorVector2d.
*/
const double coef_TriangleD2 [9][6] = {
//   0    1    2    3    4    5    
  {  0.,  0.,  0.,  0.,  1.,  0.},
  {-0.5,  0.,  0.,  0.,-0.5,  2.},
  {  1.,  0.,  0.,  0.,  0.,  0.},
  {  0.,  0.,-0.5,  2.,-0.5,  0.},
  {-0.5,  1.,-0.5,  1., -1.,  1.},
  {  1.,  2.,-0.5,  0., 0.5, -2.},
  {  0.,  0.,  1.,  0.,  0.,  0.},
  {-0.5,  2.,  1., -2., 0.5,  0.},
  {  1.,  4.,  1., -4.,  3., -4.}};

/*!
  Transformation matrix to transform a Diana shape to the Bezier format.
  Color information can be transformed likewise.

  Applicable to third order triangular shapes.

  \sa coef_TriangleD2, coef_TriangleD4, setCoordinatesVector2d, setColorVector2d.
*/
const double coef_TriangleD3 [16][9] = {
//       0        1        2        3        4        5        6        7        8
  {      0.,      0.,      0.,      0.,      0.,      0.,      1.,      0.,      0.},
  { 0.33333,      0.,      0.,      0.,      0.,      0.,-0.83333,      3.,    -1.5},
  {-0.83333,      0.,      0.,      0.,      0.,      0., 0.33333,    -1.5,      3.},
  {      1.,      0.,      0.,      0.,      0.,      0.,      0.,      0.,      0.},
  {      0.,      0.,      0., 0.33333,    -1.5,      3.,-0.83333,      0.,      0.},
  {-0.16667,    0.25,    0.25,-0.16667,   -0.25,    1.25,-1.16667,    1.25,   -0.25},
  {-1.33333,    1.25,   -0.25,-0.16667,    0.25,    0.25,     0.5,   -2.75,    3.25},
  {      1.,      3.,    -1.5, 0.33333,      0.,      0.,-0.33333,     1.5,     -3.},
  {      0.,      0.,      0.,-0.83333,      3.,    -1.5, 0.33333,      0.,      0.},
  {-0.16667,   -0.25,    1.25,-1.33333,    3.25,   -2.75,     0.5,    0.25,    0.25},
  {-0.83333,      1.,      1.,-0.83333,      2.,    -2.5, 1.66667,    -2.5,      2.},
  {     2.5,    3.75,   -0.75, 0.66667,   -0.75,   -0.75,-0.66667,    5.25,   -8.25},
  {      0.,      0.,      0.,      1.,      0.,      0.,      0.,      0.,      0.},
  { 0.33333,    -1.5,      3.,      1.,     -3.,     1.5,-0.33333,      0.,      0.},
  { 0.66667,   -0.75,    3.75,     2.5,   -8.25,    5.25,-0.66667,   -0.75,   -0.75},
  {     5.5,    2.25,    2.25,     5.5,  -15.75,   11.25,    -5.5,   11.25,  -15.75}};



/*!
  Transformation matrix to transform a Diana shape to the Bezier format.
  Color information can be transformed likewise.

  Applicable to third order triangular shapes.

  \sa coef_TriangleD3, coef_TriangleD2, setCoordinatesVector2d, setColorVector2d.
*/
const double coef_TriangleD4 [25][15] = {
//0	1	2	3	4	5	6	7	8	9	10	11	12	13	14
{0.,	0.,	0.,	0.,	0.,	0.,	0.,	0.,	1.,	0.,	0.,	0.,	0.,	0.,	0.},
{-0.25,	0.,	0.,	0.,	0.,	0.,	0.,	0.,	-1.08333,	4.,	-3.,	1.33333,	0.,	0.,	0.},
{0.72222,	0.,	0.,	0.,	0.,	0.,	0.,	0.,	0.72222,	-3.55556,	6.66667,	-3.55556,	0.,	0.,	0.},
{-1.08333,	0.,	0.,	0.,	0.,	0.,	0.,	0.,	-0.25,	1.33333,	-3.,	4.,	0.,	0.,	0.},
{1.,	0.,	0.,	0.,	0.,	0.,	0.,	0.,	0.,	0.,	0.,	0.,	0.,	0.,	0.},
{0.,	0.,	0.,	0.,	-0.25,	1.33333,	-3.,	4.,	-1.08333,	0.,	0.,	0.,	0.,	0.,	0.},
{-0.25,	0.33333,	0.25,	0.33333,	-0.25,	1.,	-1.25,	-0.33333,	-0.25,	-0.33333,	-1.25,	1.,	-2.,	-2.,	6.},
{0.72222,	-0.66667,	-0.16667,	0.66667,	-0.25,	0.66667,	-0.16667,	-0.66667,	1.13889,	-4.22223,	4.16667,	-2.88889,	2.66667,	-2.66667,	2.66667},
{-1.08333,	-0.33333,	-1.25,	1.,	-0.25,	0.33333,	0.25,	0.33333,	-0.91667,	4.33333,	-7.75,	4.33333,	6.,	-2.,	-2.},
{1.,	4.,	-3.,	1.33333,	-0.25,	0.,	0.,	0.,	0.25,	-1.33333,	3.,	-4.,	0.,	0.,	0.},
{0.,	0.,	0.,	0.,	0.72222,	-3.55556,	6.66667,	-3.55556,	0.72222,	0.,	0.,	0.,	0.,	0.,	0.},
{-0.25,	0.66667,	-0.16667,	-0.66667,	0.72222,	-2.88889,	4.16667,	-4.22223,	1.13889,	-0.66667,	-0.16667,	0.66667,	-2.66667,	2.66667,	2.66667},
{0.72222,	-1.33333,	0.11111,	-1.33333,	0.72222,	-2.22222,	2.11111,	-0.44444,	0.55556,	-0.44444,	2.11111,	-2.22222,	4.44445,	4.44445,	-6.22223},
{-1.08333,	-0.66667,	0.83333,	-2.,	0.72222,	-1.55556,	0.5,	2.44445,	-2.36111,	8.66667,	-9.16667,	4.66667,	5.33334,	5.33334,	-10.66668},
{1.,	8.00001,	2.,	-2.66667,	0.72222,	-0.88889,	-0.66667,	-0.88889,	1.72222,	-8.00001,	14.00001,	-8.00001,	-16.00001,	5.33334,	5.33334},
{0.,	0.,	0.,	0.,	-1.08333,	4.,	-3.,	1.33333,	-0.25,	0.,	0.,	0.,	0.,	0.,	0.},
{-0.25,	1.,	-1.25,	-0.33333,	-1.08333,	4.33333,	-7.75,	4.33333,	-0.91667,	0.33333,	0.25,	0.33333,	-2.,	6.,	-2.},
{0.72222,	-2.,	0.83333,	-0.66667,	-1.08333,	4.66667,	-9.16667,	8.66667,	-2.36111,	2.44445,	0.5,	-1.55556,	5.33334,	5.33334,	-10.66668},
{-1.08333,	-1.,	6.25,	-1.,	-1.08333,	5.,	-7.25,	6.33333,	-3.25,	6.33333,	-7.25,	5.,	-2.,	-2.,	-2.},
{1.,	12.,	14.99999,	-1.33333,	-1.08333,	5.33333,	-2.,	-10.66666,	8.41666,	-30.66665,	32.99999,	-12.,	-47.99998,	-15.99999,	47.99998},
{0.,	0.,	0.,	0.,	1.,	0.,	0.,	0.,	0.,	0.,	0.,	0.,	0.,	0.,	0.},
{-0.25,	1.33333,	-3.,	4.,	1.,	-4.,	3.,	-1.33333,	0.25,	0.,	0.,	0.,	0.,	0.,	0.},
{0.72222,	-2.66667,	2.,	8.00001,	1.,	-8.00001,	14.00001,	-8.00001,	1.72222,	-0.88889,	-0.66667,	-0.88889,	5.33334,	-16.00001,	5.33334},
{-1.08333,	-1.33333,	14.99999,	12.,	1.,	-12.,	32.99999,	-30.66665,	8.41666,	-10.66666,	-2.,	5.33333,	-15.99999,	-47.99998,	47.99998},
{1.,	16.,	36.,	16.,	1.,	-16.,	60.,	-80.,	35.,	-80.,	60.,	-16.,	-96.,	-96.,	160.},};



/*!
  Transformation matrix to transform a Diana shape to the Bezier format.
  Color information can be transformed likewise.

  Applicable to second order quadrilateral shapes.

  \sa coef_QuadD3, setCoordinatesVector2d, setColorVector2d.
*/
const double coef_QuadD2 [9][8] = {
//    0    1     2    3     4    5     6    7
  {   1.,  0.,   0.,  0.,   0.,  0.,   0.,  0.},
  { -0.5,  0.,   0.,  0.,   0.,  0., -0.5,  2.},
  {   0.,  0.,   0.,  0.,   0.,  0.,   1.,  0.},
  { -0.5,  2., -0.5,  0.,   0.,  0.,   0.,  0.},
  {-0.75,  1.,-0.75,  1.,-0.75,  1.,-0.75,  1.},
  {   0.,  0.,   0.,  0., -0.5,  2., -0.5,  0.},
  {   0.,  0.,   1.,  0.,   0.,  0.,   0.,  0.},
  {   0.,  0., -0.5,  2., -0.5,  0.,   0.,  0.},
  {   0.,  0.,   0.,  0.,   1.,  0.,   0.,  0.}};

/*!
  Transformation matrix to transform a Diana shape to the Bezier format.
  Color information can be transformed likewise.

  Applicable to third order quadrilateral shapes.

  \sa coef_QuadD2, setCoordinatesVector2d, setColorVector2d.
*/
const double coef_QuadD3 [16][12] = {
//       0        1        2        3        4        5        6        7        8        9       10       11
  {      1.,      0.,      0.,      0.,      0.,      0.,      0.,      0.,      0.,      0.,      0.,      0.},
  {-0.83333,      0.,      0.,      0.,      0.,      0.,      0.,      0.,      0., 0.33333,    -1.5,      3.},
  { 0.33333,      0.,      0.,      0.,      0.,      0.,      0.,      0.,      0.,-0.83333,      3.,    -1.5},
  {      0.,      0.,      0.,      0.,      0.,      0.,      0.,      0.,      0.,      1.,      0.,      0.},
  {-0.83333,      3.,    -1.5, 0.33333,      0.,      0.,      0.,      0.,      0.,      0.,      0.,      0.},
  {-1.55556,      2.,     -1.,-0.27778,      1.,    -0.5, 0.11111,    -0.5,      1.,-0.27778,     -1.,      2.},
  {-0.27778,      1.,    -0.5, 0.11111,    -0.5,      1.,-0.27778,     -1.,      2.,-1.55556,      2.,     -1.},
  {      0.,      0.,      0.,      0.,      0.,      0., 0.33333,    -1.5,      3.,-0.83333,      0.,      0.},
  { 0.33333,    -1.5,      3.,-0.83333,      0.,      0.,      0.,      0.,      0.,      0.,      0.,      0.},
  {-0.27778,     -1.,      2.,-1.55556,      2.,     -1.,-0.27778,      1.,    -0.5, 0.11111,    -0.5,      1.},
  { 0.11111,    -0.5,      1.,-0.27778,     -1.,      2.,-1.55556,      2.,     -1.,-0.27778,      1.,    -0.5},
  {      0.,      0.,      0.,      0.,      0.,      0.,-0.83333,      3.,    -1.5, 0.33333,      0.,      0.},
  {      0.,      0.,      0.,      1.,      0.,      0.,      0.,      0.,      0.,      0.,      0.,      0.},
  {      0.,      0.,      0.,-0.83333,      3.,    -1.5, 0.33333,      0.,      0.,      0.,      0.,      0.},
  {      0.,      0.,      0., 0.33333,    -1.5,      3.,-0.83333,      0.,      0.,      0.,      0.,      0.},
  {      0.,      0.,      0.,      0.,      0.,      0.,      1.,      0.,      0.,      0.,      0.,      0.}};
