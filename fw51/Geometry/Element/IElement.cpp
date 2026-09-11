/*
 */
 /* Copyright (c) 2012 TNO DIANA BV                              Confidential */
// IElement.cpp: implementation of the IElement class.
//
//////////////////////////////////////////////////////////////////////
#include "dimple.h"
#include <cmath>
#include <set>
#include <map>
#include <algorithm>

#include "Line.h"
#include "Vector.h"
#include "IElement.h"
#include "Matrix.h"
#include "IMesh.h"
#include "INode.h"
#include "CacheInterfaceDefault.h"
#include "ElementCacheObject.h"

// These need to be included here to make the exports of the explicit template
// instantiations work that are in these headers. Failing to do so will result
// in undefined symbols when linking against the Geometry dll.
#include "InterfaceElementTempl.h"
#include "InterfaceElementPart.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

namespace geo {

IElement::IElement()
{
}

IElement::~IElement()
{
}

const IElementSet* IElement::IndexingElementSet() const
{
  return 0;
}

int IElement::Index() const
{
  //assert(false);
  return -1;
}

int IElement::PointIndex(int /*nIndex*/) const
{
  return -1;
}

size_t IElement::NrOfNodes() const
{
  // This must be overridden if the element is part of a mesh
  assert(dynamic_cast<const IMesh *> (IndexingElementSet()) == 0);

  return 0;
}

const INode& IElement::Node(int /*nIndex*/) const
{
  assert(false); // must override this if NrOfNodes() is overridden and returns non-zero

  const INode *pBogus = 0;
  return *pBogus;
}

void IElement::Node(int /*nIndex*/, const IPoint& /*point*/)
{
  assert(false); // must override this if NrOfNodes() is overridden and returns non-zero
}

const char* IElement::SideName(int /*nIndex*/) const
{
  return 0;
}

ICacheInterface& IElement::CacheInterface()
{
  IElementSet *elt_set = const_cast<IElementSet *>(IndexingElementSet());
  return elt_set ? elt_set->CacheInterface() : CCacheInterfaceDefault::Instance();
}

// Only applicable for convex 3D polygons.
bool IElement::Contains(const IPoint &/*point*/, bool /*bIncludeEdge*/) const
{
  assert( false );
  return true;
}

void IElement::Move(const IVector &vec)
{
  int nrop = NrOfPoints();
  for(int i = 0; i < nrop; i++ )
  {
  CPoint p = Point(i);
  p.Move(vec);
  Point(i, p);
  }
}

void IElement::Rotate(const IVector &vec, const double &dAngleDeg)
{
  double SinAngle;
  double CosAngle;

  double AngleRad = dAngleDeg * DEG_TO_RAD;

  SinAngle = sin(AngleRad);
  CosAngle = cos(AngleRad);

  int nrop = NrOfPoints();
  for(int i = 0; i < nrop; i++ )
  {
  CPoint p = Point(i);
  p.Rotate(vec, SinAngle, CosAngle);
  Point(i, p);
  }

}

void IElement::Transform(const IMatrix &matrix)
{
  int nrop = NrOfPoints();
  for(int i = 0; i < nrop; i++ )
  {
  CPoint p = Point(i);
  p.Transform(matrix);
  Point(i, p);
  }
}


CPoint IElement::Max() const
{
  ICacheInterface &ci = const_cast<IElement &>(*this).CacheInterface();
  CPoint max = ci.Max(Index());
  if (max.Empty())
  {
    AssertValid();
    double max_x = Point(0).X();
    double max_y = Point(0).Y();
    double max_z = Point(0).Z();

    //CPoint PointMax = Point(0);
    int nrop = NrOfPoints();
    for(int i = 1; i < nrop; i++)
    {
      max_x = std::max(max_x, Point(i).X());
      max_y = std::max(max_y, Point(i).Y());
      max_z = std::max(max_z, Point(i).Z());
    }
    max.Set(max_x, max_y, max_z);
    ci.Max(Index(), max);
  }
  return max;
  //return CPoint(max_x, max_y, max_z);
}


CPoint IElement::Min() const
{
  ICacheInterface &ci = const_cast<IElement &>(*this).CacheInterface();
  CPoint min = ci.Min(Index());
  if (min.Empty())
  {
    AssertValid();
    double min_x = Point(0).X();
    double min_y = Point(0).Y();
    double min_z = Point(0).Z();

    //CPoint Pointmin = Point(0);
    int nrop = NrOfPoints();
    for(int i = 1; i < nrop; i++)
    {
      min_x = std::min(min_x, Point(i).X());
      min_y = std::min(min_y, Point(i).Y());
      min_z = std::min(min_z, Point(i).Z());
    }
    min.Set(min_x, min_y, min_z);
    ci.Min(Index(), min);
  }
  return min;
  //return CPoint(min_x, min_y, min_z);
}

CElementPoint IElement::MidPoint() const
{
  ICacheInterface &ci = const_cast<IElement &>(*this).CacheInterface();
  CElementPoint mid = ci.Mid(Index());
  if (mid.Empty())
  {
    AssertValid();
    double mid_x = Point(0).X();
    double mid_y = Point(0).Y();
    double mid_z = Point(0).Z();

    int nrop = NrOfPoints();
    for(int i = 1; i < nrop; i++)
    {
      mid_x += Point(i).X();
      mid_y += Point(i).Y();
      mid_z += Point(i).Z();
    }
    mid = CElementPoint(*this, mid_x / nrop, mid_y / nrop, mid_z / nrop);
    ci.Mid(Index(), mid);
  }
  return mid;
  //return CElementPoint(*this, mid_x / nrop, mid_y / nrop, mid_z / nrop);
}

bool IElement::operator < (const IElement &rhs) const
{
  int nrop = NrOfPoints();

  if ( nrop != rhs.NrOfPoints() )
    return nrop < rhs.NrOfPoints();

  std::vector<const IPoint *> vec1;
  std::vector<const IPoint *> vec2;

  vec1.reserve(nrop);
  vec2.reserve(nrop);

  for(int i = 0; i < nrop; i++)
  {
    vec1.push_back(&Point(i));
    vec2.push_back(&rhs.Point(i));
  }

  ICoordinate::CCoordinateLess less;

  std::sort(vec1.begin(), vec1.end(), less);
  std::sort(vec2.begin(), vec2.end(), less);

  return std::lexicographical_compare(vec1.begin(),vec1.end(),
                    vec2.begin(),vec2.end(),
                    less);
}

std::string IElement::Type() const
{
  return std::string();
}

void IElement::AssertValid() const
{
  assert(NrOfPoints() > 0);
}

bool IElement::Empty() const
{
  return !(NrOfPoints() > 0);
}

IElement::TDoubleVec IElement::IsoShapeFunction(const IPoint &point) const
{
  TDoubleVec vcRet;

  ICacheInterface &ci = const_cast<IElement &>(*this).CacheInterface();
  if (!ci.ShapeMap(Index(), point, vcRet))
  {
    vcRet = ShapeFunction(WorldToIso(point));
    ci.SetShapeMap(Index(), point, vcRet);
  }

  return vcRet;
}

IElement::TDoubleVec IElement::ShapeFunction(const IElement::TDoubleVec& /*isocoords*/) const
{
  assert(false);
  return TDoubleVec();
}

CValue IElement::InterpolateValue(const TDoubleVec& isocoords, const std::vector<CValue> &values) const
{
  int nNodeSize = NrOfNodes();
  if( nNodeSize == 0 )
  nNodeSize = NrOfPoints();

  assert(values.size() == nNodeSize);

  int i;
  for(i = 0; i < nNodeSize; i++)
  {
    if(!values[i].Valid()) return CValue();
  }

  TDoubleVec vcShape = ShapeFunction(isocoords);
  double dRet = 0;
  for(i = 0; i < nNodeSize; i++)
  {
    dRet += vcShape[i] * values[i].Value();
  }

  return CValue(dRet);
}

CValue IElement::InterpolateValue(const IPoint& point, const std::vector<CValue>& values) const
{
  int nNodeSize = NrOfNodes();
  if( nNodeSize == 0 )
  nNodeSize = NrOfPoints();

  assert(values.size() == nNodeSize);

  int i;
  for(i = 0; i < nNodeSize; i++)
  {
    if(!values[i].Valid()) return CValue();
  }

  TDoubleVec vcShape = IsoShapeFunction(point);
  double dRet = 0;
  for(i = 0; i < nNodeSize; i++)
  {
    dRet += vcShape[i] * values[i].Value();
  }

  return CValue(dRet);
}

/*!
  Calculates isoparametric coordinates for a given world coordinate.
  The base implementation is an iterative calculation and thus very
  expensive. Override this function for specific element types to increase
  performance.
*/
IElement::TDoubleVec IElement::WorldToIso(const geo::IPoint& point) const
{
  // maximum nr of iterations to prevent infinite loop
  const int maxiter = 10;

  // get dimension of isoparametric coordinate system
  int dim = (int)IsoCoordinates()[0].size();
  TDoubleVec isoCoord( dim, 0. ); // xi [, eta [, zeta]]

  for ( int iter = 0; iter < maxiter; ++iter ) {
  CPoint estimate = IsoToWorld( isoCoord );
  CVector delta( estimate, point );
  if ( delta.SquareLength() < 1.e-8 ) break;

  // Invert matrix with derivatives to convert delta to isopara
  geo::CMatrix matJacobi = Jacobian(isoCoord);
  if(matJacobi.RowSize() != matJacobi.ColumnSize())
  {
      assert(matJacobi.RowSize() > matJacobi.ColumnSize()); // less isoparametric than world coordinates (e.g. face in 3D space)

      // increase Jacobi matrix to 3D dimension, pad with zeroes
      geo::CMatrix mat(matJacobi.RowSize(), matJacobi.RowSize());
      for(int r = 0; r < matJacobi.RowSize(); ++r)
      {
    for(int c = 0; c < matJacobi.RowSize(); ++c)
    {
          mat.Value(r, c, (c < matJacobi.ColumnSize() ? matJacobi.Value(r, c) : (r == c ? 1. : 0.)));
    }
      }

      matJacobi = mat;
  }

  CVector isoDelta = matJacobi.GetInverse() * delta;
  isoCoord[0] += isoDelta.X();

  if(dim > 1)
      isoCoord[1] += isoDelta.Y();

  if(dim > 2)
      isoCoord[2] += isoDelta.Z();
  }
  return isoCoord;
}

void IElement::MapValuesPrepare(CElementCacheObject &cached, TWeightingType nWeightingType) const
{
  assert(cached.getWeights()->size() == cached.getPoints()->size() || cached.getWeights()->size() == 0);

  if (nWeightingType == WT_NONE)
    cached.setWeights(std::vector<double>());

  MapValuesVoightPrepare(cached);
}

std::vector<CValue> IElement::MapValuesFinal(const CElementCacheObject &cached, const std::vector<CValue> &SourceValues, TWeightingType nWeightingType) const
{
  if(SourceValues.empty())
  {
    std::vector<CValue> ret(NrOfNodes());
    for(int i = 0; i < NrOfNodes(); i++) ret[i] = CValue();
    return ret;
  }

  for (std::vector<CValue>::const_iterator it = SourceValues.begin(); it != SourceValues.end(); ++it)
  {
    if (!it->Valid())
    {
      std::vector<CValue> ret(NrOfNodes());
      for(int i = 0; i < NrOfNodes(); i++) ret[i] = CValue();
      return ret;
    }
  }

  // use weight factors 1 if none provided
  //assert(WeightFactors.size() == SourceValues.size() || WeightFactors.size() == 0);

  std::vector<double> ret;

  switch(nWeightingType)
  {
  case WT_NONE:
    ret = MapValuesVoightFinal(cached, SourceValues);
    break;
  case WT_VOIGHT:
    ret = MapValuesVoightFinal(cached, SourceValues);
    break;
  case WT_REUSS:
    {
      std::vector<CValue> newSourceValues;
      newSourceValues.reserve(SourceValues.size());
      int i;
      for(i = 0; i < SourceValues.size(); i++)
      {
        if(SourceValues[i].Value() < EPS)
        {
          // invalid, only positive values allowed
          std::vector<CValue> ret1(NrOfNodes());
          for(int j = 0; j < NrOfNodes(); j++) ret1[j] = CValue();
          return ret1;
        }
        newSourceValues[i] = CValue(1 / SourceValues[i].Value());
      }
      ret = MapValuesVoightFinal(cached, newSourceValues);

      std::vector<CValue> retvals;
      for(i = 0; i < ret.size(); i++)
      {
        if(fabs(ret[i]) > EPS) retvals.push_back(CValue(1.0 / ret[i]));
        else                   retvals.push_back(CValue());
      }

      return retvals;
    }
    break;
  default:
    assert(0); // unknown weighting type
  }

  // convert from doubles to CValues
  std::vector<CValue> retvals;
  retvals.reserve(ret.size());
  for(int i = 0; i < ret.size(); i++) retvals.push_back(CValue(ret[i]));
  return retvals;
}

void IElement::MapValuesVoightPrepare(CElementCacheObject &cached) const
{
  int nrop = NrOfNodes();

  CMatrix mat(nrop, nrop);
  CMatrix rhs(nrop, 1);

  std::vector<double> *weights = cached.getWeights();
  std::vector<const IPoint *> *points = cached.getPoints();

  double dVal;
  double dWeight;
  int size = points->size();

  if (weights->size() == 0)
  {
    for (int i = 0; i < nrop; i++) // rows
    {
      for(int j = 0; j < nrop; j++) // columns
      {
        dVal = 0;
        for(int k = 0; k < size; k++) // source values
        {
          //TDoubleVec isocoords = WorldToIso(*SourceValues[k].first);
          //TDoubleVec shape = ShapeFunction(isocoords);
          TDoubleVec shape = IsoShapeFunction(*(*points)[k]);
          dVal += shape[i] * shape[j];
        }
        mat.Value(i, j, dVal);
      }
    }
  }
  else
  {
    for (int i = 0; i < nrop; i++) // rows
    {
      for(int j = 0; j < nrop; j++) // columns
      {
        dVal = 0;
        for(int k = 0; k < size; k++) // source values
        {
          dWeight = (*weights)[k];
          assert(fabs(dWeight) > EPS); // no zero weight factors allowed

          //TDoubleVec isocoords = WorldToIso(*SourceValues[k].first);
          //TDoubleVec shape = ShapeFunction(isocoords);
          TDoubleVec shape = IsoShapeFunction(*(*points)[k]);
          dVal += dWeight * shape[i] * shape[j];
        }
        mat.Value(i, j, dVal);
      }
    }
  }

  CMatrix matinv = mat.GetInverse();

  cached.setMatrix(matinv);
}

std::vector<double> IElement::MapValuesVoightFinal(const CElementCacheObject &cached, const std::vector<CValue> &SourceValues) const
{
  int nrop = NrOfNodes();

  const CMatrix *matinv = cached.getMatrix();
  const std::vector<double> *weights = cached.getWeights();
  const std::vector<const IPoint *> *points = cached.getPoints();
  
  CMatrix rhs(nrop, 1);


  double dVal;
  double dWeight;
  int size = points->size();

  if (weights->size() == 0)
  {
    for(int i = 0; i < nrop; i++) // rows
    {
      // fill right hand side
      dVal = 0;
      for(int k = 0; k < size; k++)
      {
        //TDoubleVec isocoords = WorldToIso(*SourceValues[k].first);
        //TDoubleVec shape = ShapeFunction(isocoords);
        TDoubleVec shape = IsoShapeFunction(*(*points)[k]);
        dVal += SourceValues[k].Value() * shape[i];
      }

      rhs.Value(i, 0, dVal);
    }
  }
  else
  {
    for(int i = 0; i < nrop; i++) // rows
    {
      // fill right hand side
      dVal = 0;
      for(int k = 0; k < size; k++)
      {
        dWeight = (*weights)[k];
        //TDoubleVec isocoords = WorldToIso(*SourceValues[k].first);
        //TDoubleVec shape = ShapeFunction(isocoords);
        TDoubleVec shape = IsoShapeFunction(*(*points)[k]);
        dVal += dWeight * SourceValues[k].Value() * shape[i];
      }

      rhs.Value(i, 0, dVal);
    }
  }


  CMatrix uvec = *matinv * rhs;

  std::vector<double> ret(nrop);
  for(int i = 0; i < nrop; i++) ret[i] = uvec.Value(i, 0);

  return ret;
}



std::vector<double> IElement::MapValuesVoight(TSourceValueVector &SourceValues, const std::vector<double> &WeightFactors) const
{
  int nrop = NrOfNodes();

  CMatrix mat(nrop, nrop);
  CMatrix rhs(nrop, 1);

  int i;
  for(i = 0; i < nrop; i++) // rows
  {
  for(int j = 0; j < nrop; j++) // columns
  {
      double dVal = 0;
      for(int k = 0; k < SourceValues.size(); k++) // source values
      {
    double dWeight = 1;
    if(WeightFactors.size() != 0) dWeight = WeightFactors[k];
    assert(fabs(dWeight) > EPS); // no zero weight factors allowed

    TDoubleVec shape = IsoShapeFunction(*SourceValues[k].first);
    dVal += dWeight * shape[i] * shape[j];
      }
      mat.Value(i, j, dVal);
  }

  // fill right hand side
  double dVal = 0;
  for(int k = 0; k < SourceValues.size(); k++)
  {
      double dWeight = 1;
      if(WeightFactors.size() != 0) dWeight = WeightFactors[k];
      TDoubleVec shape = IsoShapeFunction(*SourceValues[k].first);
      dVal += dWeight * SourceValues[k].second.Value() * shape[i];
  }

  rhs.Value(i, 0, dVal);
  }

  CMatrix matinv = mat.GetInverse();
  CMatrix uvec = matinv * rhs;

  std::vector<double> ret(nrop);
  for(i = 0; i < nrop; i++) ret[i] = uvec.Value(i, 0);

  return ret;
}

double IElement::ReussDerived(TSourceValueVector &SourceValues, const std::vector<double> &WeightFactors, const IMatrix &NodeValues, int iIndex) const
{
  int nrop = NrOfNodes();
  assert(iIndex >= 0 && iIndex < nrop);
  assert(NodeValues.RowSize() == nrop && NodeValues.ColumnSize() == 1);
  assert(WeightFactors.size() == 0 || WeightFactors.size() == SourceValues.size());

  int i, j;

  // Compute weightfactors
  std::vector<double> Weight(SourceValues.size());
  if(WeightFactors.size() > 0)
  {
  // normalize to 1
  double dSum = 0;
  for(i = 0; i < WeightFactors.size(); i++) dSum += WeightFactors[i];
  assert(dSum != 0);
  for(i = 0; i < WeightFactors.size(); i++) Weight[i] = WeightFactors[i] / dSum;
  }
  else
  {
  // no weightfactors, equally weighted, normalize to 1
  for(i = 0; i < SourceValues.size(); i++)
  {
      Weight[i] = 1. / (double) SourceValues.size();
  }
  }

  // first term
  double dFirst;

  double iSum = 0;
  for(i = 0; i < SourceValues.size(); i++)
  {
  double jSum = 0;
  for(j = 0; j < nrop; j++)
  {
      TDoubleVec shape = IsoShapeFunction(*SourceValues[i].first);
      jSum += shape[j] * NodeValues.Value(j, 0);
  }
  double Counter = pow(SourceValues[i].second.Value() - jSum, 2);
  iSum += Weight[i] / Counter;
  }
  dFirst = -2. / pow(iSum, 2);

  // second term
  double dSecond;

  iSum = 0;
  for(i = 0; i < SourceValues.size(); i++)
  {
  double jSum = 0;
  for(j = 0; j < nrop; j++)
  {
      TDoubleVec shape = IsoShapeFunction(*SourceValues[i].first);
      jSum += shape[j] * NodeValues.Value(j, 0);
  }
  double Counter = pow(SourceValues[i].second.Value() - jSum, 3);
  TDoubleVec shape = IsoShapeFunction(*SourceValues[i].first);
  iSum += Weight[i] * shape[iIndex] / Counter;
  }
  dSecond = iSum;

  return dFirst * dSecond;
}

#define VSV 1e-8 // Very Small Value

std::vector<double> IElement::MapValuesReuss(TSourceValueVector &SourceValues, const std::vector<double> &WeightFactors) const
{
  int nrop = NrOfNodes();
  CMatrix u(nrop, 1);
  CMatrix J(nrop, nrop);
  CMatrix rhs(nrop, 1);

  CMatrix du(nrop, 1);

  // wed:TODO better initial guess?
  int i;
  for(i = 0; i < nrop; i++) u.Value(i, 0, 100);
  bool bConverged = false;

  do
  {
  for(int row = 0; row < nrop; row++)
  {
      // avoid making copies, get reference to vector in matrix u
      double dF = ReussDerived(SourceValues, WeightFactors, u, row);
      rhs.Value(row, 0, dF);

      for(int col = 0; col < nrop; col++)
      {
    // remember value of u[row][0]
    double dBak = u.Value(col, 0);
    // add small delta for derived
    u.Value(col, 0, u.Value(col, 0) + VSV);
    // get function value for added small delta
    double dFdelta = ReussDerived(SourceValues, WeightFactors, u, row);
    // reset to backed up value
    u.Value(col, 0, dBak);
    double dDeltaF = dFdelta - dF;
    J.Value(row, col, dDeltaF / VSV);
      }
  }

  du = J.GetInverse() * rhs * (-1);

  bConverged = true;
  for(i = 0; i < nrop; i++)
  {
      if(fabs(du.Value(i, 0)) > (10 * VSV)) bConverged = false;
  }

  u = u + du;
  }
  while(!bConverged);

  std::vector<double> ret(nrop);
  for(i = 0; i < nrop; i++)
  {
  ret[i] = u.Value(i, 0);
  }

  return ret;
/*
  CMatrix u(NrOfPoints(), 1);
  for(int i = 0; i < NrOfPoints(); i++) u[i][0] = 0;

  bool bFinished;
  double dWeightMin;

  double dWeightTotal = 0;
  for(i = 0; i < SourceValues.size(); i++)
  {
  double dWeight = 1;
  if(WeightFactors.size() != 0) dWeight = WeightFactors[i];

  if(!i) dWeightMin = fabs(dWeight);
  else   dWeightMin = std::min(fabs(dWeight), dWeightMin);

  dWeightTotal += dWeight;
  }

  double dFactor = dWeightTotal / dWeightMin;

  do
  {
  assert(fabs(dWeightTotal) > EPS);
  // vector of errors
  CMatrix dBeta(SourceValues.size(), 1);
  for(i = 0; i < SourceValues.size(); i++)
  {
      double dWeight = 1;
      if(WeightFactors.size() != 0) dWeight = WeightFactors[i];
      assert(fabs(dWeight) > EPS); // no zero weight factors allowed
      dWeight /= dWeightTotal;
      dWeight *= dFactor;
      assert(fabs(dWeight) >= 1);
      double delta = SourceValues[i].second;
      for(int j = 0; j < NrOfPoints(); j++) delta -= ShapeFunctionValue(j, *SourceValues[i].first) * u[j][0];
      delta /= dWeight;
      dBeta[i][0] = delta;
  }

  CMatrix A(SourceValues.size(), NrOfPoints());
  for(i = 0; i < SourceValues.size(); i++)
  {
      for(int j = 0; j < NrOfPoints(); j++)
      {
    A[i][j] = ShapeFunctionValue(j, *SourceValues[i].first);
      }
  }

  CMatrix AT = A.GetTransposed();
  CMatrix du = (AT * A).GetInverse() * AT * dBeta;

  bFinished = true;
  for(i = 0; i < NrOfPoints(); i++)
  {
      if(fabs(du[i][0]) > 1e-8) bFinished = false;
  }

  u = u + du;
  double ddum = du[0][0];
  double dum = u[0][0];
  } while(!bFinished);

  std::vector<double> ret(NrOfPoints());

  for(i = 0; i < NrOfPoints(); i++) ret[i] = u[i][0];

  return ret;
*/
}

std::vector<CValue> IElement::MapValues(TSourceValueVector &SourceValues, TWeightingType nWeightingType, const std::vector<double> &WeightFactors) const
{
  // return vector with invalid values if any source value is invalid
  for(TSourceValueVector::iterator it = SourceValues.begin(); it != SourceValues.end(); it++)
  {
  if(!it->second.Valid())
  {
      std::vector<CValue> ret(NrOfNodes());
      for(int i = 0; i < NrOfNodes(); i++) ret[i] = CValue();
      return ret;
  }
  }

  // no source values at all
  if(SourceValues.empty())
  {
  std::vector<CValue> ret(NrOfNodes());
  for(int i = 0; i < NrOfNodes(); i++) ret[i] = CValue();
  return ret;
  }

  // use weight factors 1 if none provided
  assert(WeightFactors.size() == SourceValues.size() || WeightFactors.size() == 0);

  std::vector<double> ret;

  switch(nWeightingType)
  {
  case WT_NONE:
  ret = MapValuesVoight(SourceValues, std::vector<double> ());
  break;
  case WT_VOIGHT:
  ret = MapValuesVoight(SourceValues, WeightFactors);
  break;
  case WT_REUSS:
  {
      int i;
      for(i = 0; i < SourceValues.size(); i++)
      {
    if(SourceValues[i].second.Value() < EPS)
    {
          // invalid, only positive values allowed
          std::vector<CValue> ret1(NrOfNodes());
          for(int j = 0; j < NrOfNodes(); j++) ret1[j] = CValue();
          return ret1;
    }
    SourceValues[i].second = CValue(1 / SourceValues[i].second.Value());
      }
      ret = MapValuesVoight(SourceValues, WeightFactors);

      std::vector<CValue> retvals;
      for(i = 0; i < ret.size(); i++)
      {
    if(fabs(ret[i]) > EPS) retvals.push_back(CValue(1.0 / ret[i]));
    else                   retvals.push_back(CValue());
      }

      return retvals;
  }
  break;
  default:
  assert(0); // unknown weighting type
  }

  // convert from doubles to CValues
  std::vector<CValue> retvals;
  for(int i = 0; i < ret.size(); i++) retvals.push_back(CValue(ret[i]));
  return retvals;
}

std::vector<CValue> IElement::MapValues(TSourceValueVector &SourceValues, TWeightingType nWeightingType) const
{
  std::vector<double> vecDumWeights;
  return MapValues(SourceValues, nWeightingType, vecDumWeights);
}

std::vector<int> IElement::OverlappingElementIndices(std::vector<const IElement*> &/*vecElements*/) const
{
  std::vector<int> ret;
  assert(false);

  return ret;
}

void IElement::InsertCorners(const geo::IElement &elem, geo::CPtrArray<geo::IPoint> &result) const
{
  int j;

  for(j = 0; j < NrOfPoints(); j++)
  if(elem.Contains(Point(j), true)) result.PushBack(*(new geo::CPoint(Point(j))));

  for(j = 0; j < elem.NrOfPoints(); j++)
  if(Contains(elem.Point(j), true)) result.PushBack(*(new geo::CPoint(elem.Point(j))));
}

bool IElement::ContainsInvalidValue(const std::vector<CValue> &values) const
{
  for(std::vector<CValue>::const_iterator it = values.begin(); it != values.end(); it++)
  if(!it->Valid()) return true;

  return false;
}

double IElement::InfluenceSize(int /*nNode*/) const
{
  assert(false);

  return 0.0;
}

/*!
  Returns a "relative" EPS for isoparametric coordinates.
*/
double IElement::IsoEps() const
{
//  return EPS;
  double l = RepresentativeLength();
  if(l < 1.0) l = 1.0;
  return EPS / l;
}

/*!
  The minimum and maximum angles between two edges in a point (mesh quality).
  - first is minimum
  - second is maximum
*/
std::pair<double, double> IElement::MinMaxAngles() const
{
  typedef std::vector<const ILine*> TLineVector;
  typedef std::map<CPoint, TLineVector> TLineMap;

  // the lines attached to each point of the element
  TLineMap linemap;

  // walk over the lines and put each line in the map for the end-points
  int i;
  for(i = 0; i < NrOfLines(); i++)
  {
  const ILine *pLine = &Line(i);
  std::pair<TLineMap::iterator, bool> prInsert;
  prInsert = linemap.insert(std::make_pair(CPoint(pLine->Point(0)), std::vector<const ILine*>()));
  prInsert.first->second.push_back(pLine);
  prInsert = linemap.insert(std::make_pair(CPoint(pLine->Point(1)), std::vector<const ILine*>()));
  prInsert.first->second.push_back(pLine);
  }

  std::pair<double, double> ret(PI, 0);

  for(TLineMap::iterator it = linemap.begin(); it != linemap.end(); it++)
  {
  const IPoint &pt = it->first;

  for(int l1 = 0; l1 < it->second.size() - 1; l1++)
  {
      const ILine &ln1 = *it->second[l1];
      CVector vecl1(ln1);
      vecl1 = vecl1.UnitVector();
      if(ln1.First() != pt) vecl1.Flip();

      for(int l2 = l1 + 1; l2 < it->second.size(); l2++)
      {
    const ILine &ln2 = *it->second[l2];
    CVector vecl2(ln2);
    vecl2 = vecl2.UnitVector();
    if(ln2.First() != pt) vecl2.Flip();

    double dot = vecl1.DotProduct(vecl2);
    assert(dot >= -1 && dot <= 1);

    double angle = acos(dot);
    assert(angle >= 0 && angle <= PI);
    if(angle < ret.first ) ret.first  = angle;
    if(angle > ret.second) ret.second = angle;
      }
  }
  }

  ret.first  *= 180.0 / PI;
  ret.second *= 180.0 / PI;

  return ret;
}

/*!
  Replace the node with local index nIndex by the node with mesh node index
  nNewGlobalMeshNodeIndex.
  This replaces the node only locally in this element, not globally in the mesh.
*/
void IElement::ReplaceNode(int /*nIndex*/, int /*nNewGlobalMeshNodeIndex*/)
{
  assert(false); // needs implementation in derived class
}

/*!
  This function must be overridden for proper interpolation and integration
  (default Size() implementation).
  It asserts false if called.
*/
CMatrix IElement::ShapeFunctionDerived(const IElement::TDoubleVec& /*isocoords*/) const
{
  assert(false);
  return CMatrix();
}

/*!
  Returns a matrix with the nodal coordinates (including higher order nodes) of the element.
  The row direction walks over the directions (x, y, z).
  The column direction walks over the nodes.
*/
CMatrix IElement::Coordinates() const
{
  int nNodeSize = NrOfNodes();
  if( nNodeSize == 0 )
  nNodeSize = NrOfPoints();

  CMatrix coords(3, nNodeSize);

  int i;
  for(i = 0; i < nNodeSize; i++)
  {
  if(NrOfNodes() == 0)
  {
      coords.Value(0, i, Point(i).X());
      coords.Value(1, i, Point(i).Y());
      coords.Value(2, i, Point(i).Z());
  }
  else
  {
      coords.Value(0, i, Node(i).X());
      coords.Value(1, i, Node(i).Y());
      coords.Value(2, i, Node(i).Z());
  }
  }

  return coords;
}

/*!
  The Jacobian matrix contains the derivatives of the world coordinates
  to the isoparametric coordinates.
  It can be calculated by multiplying the matrix with the derivatives
  of the shape functions with the transposed of the nodal coordinate
  matrix (Zienkiewicz & Taylor, Finite Element Method 4th ed. vol. 1,
  paragraph 8.5).
*/
CMatrix IElement::Jacobian(const IElement::TDoubleVec& isocoords, const IMatrix& matcoords) const
{
//  int i;
//  for(i = 0; i < isocoords.size(); i++)
//    assert(isocoords[i] >= -1.0 && isocoords[i] <= 1.0);

  CMatrix P = ShapeFunctionDerived(isocoords);

  assert(P.RowSize() > 0 && P.RowSize() <= 3);

  return matcoords * P.GetTransposed();
}

CMatrix IElement::Jacobian(const IElement::TDoubleVec& isocoords) const
{
  CMatrix coords = Coordinates();

  return Jacobian(isocoords, coords);
}

/*!
  This "determinant" depends on the matrix's dimension, which is either 1x3, 2x3 or 3x3.
  - for 1x3 it returns the length of the vector
  - for 2x3 it calculates the cross product of the two vectors and returns the length of the result
  - for 3x3 it returns the real determinant of the matrix
*/
double IElement::JacobianDeterminant(const IMatrix& J) const
{
  assert(J.RowSize() == 3);
  assert(J.ColumnSize() > 0 && J.ColumnSize() <= 3);

  switch(J.ColumnSize())
  {
  case 1:
  return CVector(J.Value(0, 0), J.Value(1, 0), J.Value(2, 0)).Length();
  case 2:
  return CVector(J.Value(0, 0), J.Value(1, 0), J.Value(2, 0)).CrossProduct(CVector(J.Value(0, 1), J.Value(1, 1), J.Value(2, 1))).Length();
  }
  return J.GetDeterminant();
}

/*!
  This generic implementation relies on integration points.
  If no integration scheme has been implemented, this function should be overridden.

  Each integration point contributes to the size, taking the weight factors into account.

  The size is equal to the sum of the Jacobian determinant values times the weight factor for each integration point.
*/
double IElement::Size() const
{
  assert(IntegrationPointSize() > 0);

  double dSize = 0;

  CMatrix coords = Coordinates();

  int i;
  for(i = 0; i < IntegrationPointSize(); i++)
  {
  CMatrix J = Jacobian(IntegrationPointCoords(i), coords);
  dSize += JacobianDeterminant(J) * IntegrationPointWeight(i);
  }

  return dSize;
}

std::vector<IElement::TDoubleVec> IElement::IsoCoordinates() const
{
  assert(false);
  return std::vector<TDoubleVec>();
}

/*!
  This function must be overridden for proper interpolation and integration (default Size() implementation).
  It always returns zero.
*/
int IElement::IntegrationPointSize() const
{
  return 0;
}

/*!
  This function must be overridden for proper interpolation and integration (default Size() implementation).
  It asserts false if called and returns bogus.
*/
const IElement::TDoubleVec& IElement::IntegrationPointCoords(int /*nIndex*/) const
{
  assert(false);
  TDoubleVec* pBogus = 0;
  return *pBogus;
}

/*!
  This function must be overridden for proper interpolation and integration (default Size() implementation).
  It asserts false if called and returns bogus.
*/
const double& IElement::IntegrationPointWeight(int /*nIndex*/) const
{
  assert(false);
  double* pBogus = 0;
  return *pBogus;
}

IElement::TDoubleVec IElement::MakeVec(const double& d1, const double& d2, const double& d3) const
{
  TDoubleVec ret;
  ret.push_back(d1);
  ret.push_back(d2);
  ret.push_back(d3);

  return ret;
}

IElement::TDoubleVec IElement::MakeVec(const double& d1, const double& d2) const
{
  TDoubleVec ret;
  ret.push_back(d1);
  ret.push_back(d2);

  return ret;
}

CPoint IElement::IsoToWorld(const TDoubleVec& isocoords) const
{
  int nNodeSize = NrOfNodes();
  if( nNodeSize == 0 )
  nNodeSize = NrOfPoints();

  TDoubleVec shape = ShapeFunction(isocoords);
  assert(shape.size() == nNodeSize);

  double x = 0;
  double y = 0;
  double z = 0;

  int i;
  for(i = 0; i < nNodeSize; i++)
  {
  if(NrOfNodes() == 0)
  {
      x += shape[i] * Point(i).X();
      y += shape[i] * Point(i).Y();
      z += shape[i] * Point(i).Z();
  }
  else
  {
      x += shape[i] * Node(i).X();
      y += shape[i] * Node(i).Y();
      z += shape[i] * Node(i).Z();
  }

  }

  return CPoint(x, y, z);
}

bool IElement::IsElement() const
{
  return true;
}

}
