 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */
// IValueSet.h: interface for the IValueSet class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IVALUESET_H__4FF239ED_260F_41D7_B6D5_9E381BEA2D83__INCLUDED_)
#define AFX_IVALUESET_H__4FF239ED_260F_41D7_B6D5_9E381BEA2D83__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "IValueSetTemplate.h"

#include "GeometryExports.h"

class IProgressBase;
class CElementCacheObject;

namespace geo {


class GEOMETRY_EXPORT  IValueSet : public IValueSetTemplate<CValue> 
{
public:
  IValueSet();
  virtual ~IValueSet();

  // get the nodal value for the given node index using the given weighting (always smoothed)
  virtual CValue NodeValue(int iNodeIndex, TWeightingType nWeightingType, IParallelInitializationCallback *cb) const;

  // get the value at the given point using the given weighting (not smoothed, but averaged when in two or more elements)
  virtual CValue PointValue(const IPoint &point, TWeightingType nWeightingType, IParallelInitializationCallback *cb) const;

  // get the value at the given point using the given weighting, but only if (partially) overlapping with the given element
  // (not smoothed, but averaged when in two or more elements)
  virtual CValue PointValue(const IPoint &point, TWeightingType nWeightingType, const IElement &Overlap, IParallelInitializationCallback *cb) const;

  // get the smoothed value at the given point, where nodal smoothing is performed using the given weighting type
  virtual CValue PointValueSmoothed(const IPoint &point, TWeightingType nWeightingType, IParallelInitializationCallback *cb) const;

  // empty the set
  virtual void Empty() = 0;
  // check for emptyness
  virtual bool IsEmpty() const = 0;

  virtual bool IsValid(const CValue& value) const;

  // map values from this valueset onto the given element, put the values into the given vector (must be empty)
  virtual bool MapValues(const IBody &body, std::vector<CValue> &values, TWeightingType nWeightingType, IParallelInitializationCallback *cb) const;


  // get nodal values for the element from smoothed values in the set
  virtual bool GetSmoothedNodalValues(const IElement &element, std::vector<CValue> &values, IParallelInitializationCallback *cb) const;

  // map values from this valueset (and its elementset) to the given valueset (and its elementset)
  virtual void MapValues(IValueSet &TargetValues, TWeightingType nWeightingType, IParallelInitializationCallback *cb, IProgressBase *pProgress = 0) const;
protected:
  // protected interface to set the values for the given element
  virtual int PushBack(const TValueVec &values) = 0;

private:
  std::vector<double> WeightFactors(const std::vector<int> &vecElements) const;
  CValue GetWeightedAverage(const std::vector<CValue> &values, const std::vector<double> &weightfactors, TWeightingType nWeightingType) const;
  CValue PointValue(const IPoint &point, std::vector<int> &vecElements, TWeightingType nWeightingType, IParallelInitializationCallback *cb) const;
  bool MapValuesWithCache(CElementCacheObject &cached, const IBody &body, std::vector<CValue> &values, TWeightingType nWeightingType, IParallelInitializationCallback *cb) const;
};

}

#endif // !defined(AFX_IVALUESET_H__4FF239ED_260F_41D7_B6D5_9E381BEA2D83__INCLUDED_)
