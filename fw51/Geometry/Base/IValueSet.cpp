 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */
// IValueSet.cpp: implementation of the IValueSet class.
//
//////////////////////////////////////////////////////////////////////

#include "dimple.h"
#include <cmath>
#include <map>
#include "IElementSet.h"
#include "IBody.h"
#include "IValueSet.h"
#include "IProgressBase.h"
#include "BodyQuadrilateral.h"
#include "BodyTriangle.h"
#include "EmbeddedTriangle.h"
#include "ElementCacheObject.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

namespace geo {

IValueSet::IValueSet()
{

}

IValueSet::~IValueSet()
{

}

std::vector<double> IValueSet::WeightFactors(const std::vector<int> &vecElements) const
{
  double Total = 0;
  for(std::vector<int>::const_iterator it = vecElements.begin(); it != vecElements.end(); it++) Total += ElementSet().Element(*it).Size();

  std::vector<double> ret(vecElements.size());
  for(size_t i = 0; i < vecElements.size(); i++) ret[i] = ElementSet().Element(vecElements[i]).Size() / Total;

  return ret;
}

CValue IValueSet::GetWeightedAverage(const std::vector<CValue> &values, const std::vector<double> &weightfactors, TWeightingType nWeightingType) const
{
  for(size_t i = 0; i < values.size(); i++)
    if(!values[i].Valid()) return CValue();

  CValue total(0);
  double weighttotal = 0;
  CValue retval;

  assert(values.size() != 0);

  switch(nWeightingType)
  {
  case WT_NONE:
    {
      for(std::vector<CValue>::const_iterator it = values.begin(); it != values.end(); it++) total += *it;
      retval = total / values.size();
    }
    break;
  case WT_VOIGHT:
    {
      assert(weightfactors.size() == values.size());
      std::vector<double>::const_iterator itw;
      std::vector<CValue>::const_iterator itv;
      for(itw = weightfactors.begin(), itv = values.begin(); itw != weightfactors.end(); itw++, itv++)
      {
        weighttotal += *itw;
        total += itv->Value() * *itw;
      }

      assert(fabs(weighttotal) > EPS);
      retval = total / weighttotal;
    }
    break;
  case WT_REUSS:
    {
      assert(weightfactors.size() == values.size());
      std::vector<CValue> vcInvValues(values.size());
      for(size_t i = 0; i < values.size(); i++)
      {
        if(fabs(values[i].Value()) < EPS) return CValue(); // must be able to invert
        vcInvValues[i] = CValue(1.0 / values[i].Value());
      }

      CValue InvRet = GetWeightedAverage(vcInvValues, weightfactors, WT_VOIGHT);
      if(!InvRet.Valid() || fabs(InvRet.Value()) < EPS) return CValue();

      retval = CValue(1.0 / InvRet.Value());
    }
    break;
  default:
    assert(false); // unknown weighting type
  }

  return retval;
}

bool IValueSet::IsValid(const CValue& value) const
{
  return value.Valid();
}

CValue IValueSet::PointValue(const IPoint &point, std::vector<int> &vecElements, TWeightingType nWeightingType, IParallelInitializationCallback *cb) const
{
  CValue retval = 0;

  if(vecElements.size() == 1)
  {
    // single element found, simply interpolate
    retval = InterpolateValue(point, vecElements[0], cb);
  }
  else
  {
    // multiple elements found, average based on weighting
    std::vector<double> weightfactors;

    if(nWeightingType != WT_NONE)
    {
      weightfactors = WeightFactors(vecElements);
      assert(weightfactors.size() == vecElements.size());
    }

    std::vector<CValue> elmvalues(vecElements.size());
    for(size_t i = 0; i < vecElements.size(); i++)
      elmvalues[i] = InterpolateValue(point, vecElements[i], cb);

    retval = GetWeightedAverage(elmvalues, weightfactors, nWeightingType);
  }

  return retval;
}

// get the value at the given point using the given weighting (not smoothed, but averaged when in two or more elements)
CValue IValueSet::PointValue(const IPoint &point, TWeightingType nWeightingType, IParallelInitializationCallback *cb) const
{
  std::vector<int> vecElements = ElementSet().ElementsAt(point);
  assert(vecElements.size() > 0); // must give point inside elementset

  return PointValue(point, vecElements, nWeightingType, cb);
}

// get the value at the given point using the given weighting, but only if (partially) overlapping with the given element
// (not smoothed, but averaged when in two or more elements)
CValue IValueSet::PointValue(const IPoint &point, TWeightingType nWeightingType, const IElement &Overlap, IParallelInitializationCallback *cb) const
{
  std::vector<int> vecElements = ElementSet().ElementsAt(point);
  assert(vecElements.size() > 0); // must give point inside elementset

  std::vector<const IElement*> elements(vecElements.size());
  for(size_t i = 0; i < vecElements.size(); i++) elements[i] = &ElementSet().Element(vecElements[i]);

  std::vector<int> vecOverlapping = Overlap.OverlappingElementIndices(elements);
  std::vector<int> newvec(vecOverlapping.size());
  for(size_t i = 0; i < vecOverlapping.size(); i++) newvec[i] = vecElements[vecOverlapping[i]];

  return PointValue(point, newvec, nWeightingType, cb);
}

// get the smoothed value at the given point, where nodal smoothing is performed using the given weighting type
CValue IValueSet::PointValueSmoothed(const IPoint &point, TWeightingType nWeightingType, IParallelInitializationCallback *cb) const
{
  std::vector<int> vecElements = ElementSet().ElementsAt(point);
  assert(vecElements.size() > 0); // must give point inside elementset

  // simply take the first element, values should be equal for all elements in the vector
  const IElement &elm = ElementSet().Element(vecElements[0]);
  std::vector<int> nodes = ElementSet().Nodes(elm);
  std::vector<CValue> values(nodes.size());

  for(size_t i = 0; i < nodes.size(); i++) values[i] = NodeValue(nodes[i], nWeightingType, cb);

  return elm.InterpolateValue(point, values);
}

// get the nodal value for the given node index using the given weighting (always smoothed)
CValue IValueSet::NodeValue(int iNodeIndex, TWeightingType nWeightingType, IParallelInitializationCallback *cb) const
{
  IElementSet::TElementNodeVec vcElementNodes = ElementSet().ElementsAt(iNodeIndex);
  std::vector<CValue> values(vcElementNodes.size());
  std::vector<double> weights(vcElementNodes.size());

  for(size_t i = 0; i < vcElementNodes.size(); i++)
  {
    TValueVec vcSource( ValueSize( vcElementNodes[i].first->Index() ) );
    ElementValues( vcSource, vcElementNodes[i].first->Index(), cb );
    values[i] = vcSource[vcElementNodes[i].second];
    weights[i] = vcElementNodes[i].first->Size();
  }

  return GetWeightedAverage(values, weights, nWeightingType);
}

// map values from this valueset onto the given element, put the values into the given vector (must be empty)
bool IValueSet::MapValues(const IBody &body, std::vector<CValue> &values, TWeightingType nWeightingType, IParallelInitializationCallback *cb) const
{
  // If we "know" the element we return the stored values
  if(body.IndexingElementSet() == &ElementSet())
  {
    ElementValues(values, body.Index(), cb);
    return true;
  }

	ICacheInterface &ci = const_cast<IElementSet *>(body.IndexingElementSet())->CacheInterface();
	CElementCacheObject *cached = ci.ElementCacheObject(&ElementSet(), &body);

	if (cached)
		return MapValuesWithCache(*cached, body, values, nWeightingType, cb);


//  const IElementSet &elset = ElementSet();

  IElement::TSourceValueVector vcSourceValues;
  std::vector<double> vcWeights;

  std::set<int> stIndex = ElementSet().Candidates(body.Min(), body.Max());
  if(stIndex.empty()) return false;

  bool bIntersectionFound = false;

  for(std::set<int>::iterator it = stIndex.begin(); it != stIndex.end(); it++)
  {
    // We only consider bodies ...
    const IBody *pBody = dynamic_cast<const IBody*>(&ElementSet().Element(*it));
    if(pBody)
    {
      TValueVec vcSrcValues( ValueSize( *it ));
      ElementValues(vcSrcValues, *it, cb);

      if(body.BoundingBoxIntersect(*pBody, false))
      {
        IBody::TIntersection Intersect = body.Intersection(*pBody);

        double dFraction = Intersect.first / pBody->Size();
        if(dFraction > EPS)
        {
          bIntersectionFound = true;
          if(fabs(dFraction) >= 1)
          {
            // pBody is completely contained inside body
            for(int j = 0; j < pBody->NrOfPoints(); j++)
            {
              assert(body.Contains(pBody->Point(j), true));
              CValue val = vcSrcValues[j];
              vcSourceValues.push_back(IElement::TValuePoint(new CPoint(pBody->Point(j)), val));
              vcWeights.push_back(1.0 / pBody->NrOfPoints());
            }
          }
          else
          {
            // only part of pBody is inside body
            for(size_t j = 0; j < Intersect.second.size(); j++)
            {
              CValue val = pBody->InterpolateValue(Intersect.second[j], vcSrcValues);
              vcSourceValues.push_back(IElement::TValuePoint(new CPoint(Intersect.second[j]), val));
              vcWeights.push_back(dFraction / Intersect.second.size());
            }
          }
        }
      }
    }
  }

  values = body.MapValues(vcSourceValues, nWeightingType, vcWeights);
  for(size_t i = 0; i < vcSourceValues.size(); i++) delete vcSourceValues[i].first;

  if(!bIntersectionFound) return false;

  return true;
}

bool IValueSet::MapValuesWithCache(CElementCacheObject &cached, const IBody &body, std::vector<CValue> &values, TWeightingType nWeightingType, IParallelInitializationCallback *cb) const
{
	std::vector<const IPoint *> vcPoints;
	std::vector<double> vcWeights;

	// Check if we have done the mapping
	ICacheInterface &ci = const_cast<IElementSet *>(body.IndexingElementSet())->CacheInterface();
    
	if (!cached.Valid() || cached.Type() != nWeightingType)
	{
		std::set<int> stIndex = ElementSet().Candidates(body.Min(), body.Max());
		if(stIndex.empty()) return false;  // since Candidates is cached, we're not caching this

		cached.Clear();
		cached.Type(nWeightingType);

		for(std::set<int>::iterator it = stIndex.begin(); it != stIndex.end(); it++)
		{
			// We only consider bodies ...
			const IBody *pBody = dynamic_cast<const IBody*>(&ElementSet().Element(*it));
			if(pBody)
			{
				if(body.BoundingBoxIntersect(*pBody, false))
				{
					IBody::TIntersection Intersect = body.Intersection(*pBody);

					double dFraction = Intersect.first / body.Size();
					if(dFraction > EPS)
					{
					  cached.addBody(pBody);
					  cached.addIntersection(Intersect);
          }
        }
      }
    }

    for (int i = 0; i < cached.NrOfBodies(); ++i)
    {
      const IBody *pBody = cached.getBody(i);
      if (pBody)
      {

        const IBody::TIntersection *pIntersect = cached.getIntersection(i);
        if (pIntersect)
        {

					double dFraction = pIntersect->first / pBody->Size();
					if(dFraction > EPS)
					{
						if(fabs(dFraction) >= 1)
						{
							// pBody is completely contained inside body
							int size = pBody->NrOfPoints();
							double weight = 1.0 / size;
							cached.addSize(size);
							for(int j = 0; j < size; j++)
							{
								assert(body.Contains(pBody->Point(j), true));
								cached.addPoint(&pBody->Point(j));
								cached.addWeight(weight);
							}
						}
						else
						{
							// only part of pBody is inside body
							int size = pIntersect->second.size();
							double weight = dFraction / size;
							cached.addSize(size);
						    for(int j = 0; j < size; j++)
							{
								cached.addPoint(&pIntersect->second[j]);
								cached.addWeight(weight);
							}
						}
			        }
				}
			}
		}

    if (cached.IntersectionFound())
		  body.MapValuesPrepare(cached, nWeightingType);

		ci.ElementCacheObject(&ElementSet(), body.Index(), cached);
	}

	std::vector<CValue> vcSourceValues;
	vcSourceValues.reserve(cached.getPoints()->size());

	std::size_t size = cached.NrOfBodies();
	for (int i = 0; i < size; ++i)
	{
		const IBody *pBody = cached.getBody(i);
		const IBody::TIntersection *Intersect = cached.getIntersection(i);

		double dFraction = Intersect->first / pBody->Size();
		if(dFraction > EPS)
		{
			TValueVec vcSrcValues( ValueSize( pBody->Index() ));
			ElementValues(vcSrcValues, pBody->Index(), cb);

			if(fabs(dFraction) >= 1)
			{
				// pBody is completely contained inside body
				for(int j = 0; j < pBody->NrOfPoints(); j++)
				{
					vcSourceValues.push_back(vcSrcValues[j]);
				}
			}
			else
			{
				// only part of pBody is inside body
			    for(size_t j = 0; j < Intersect->second.size(); j++)
				{
					vcSourceValues.push_back(pBody->InterpolateValue(Intersect->second[j], vcSrcValues));
				}
			}
        }
	}

	values = body.MapValuesFinal(cached, vcSourceValues, nWeightingType);

	return cached.IntersectionFound();
}




// get nodal values for the element from smoothed values in the set
bool IValueSet::GetSmoothedNodalValues(const IElement &element, std::vector<CValue> &values, IParallelInitializationCallback *cb) const
{
  // If we "know" the element we return the stored values
  if(element.IndexingElementSet() == &ElementSet())
  {
    ElementValues(values, element.Index(), cb);
    return true;
  }

  assert(element.NrOfPoints() == values.size());
  for(int i = 0; i < element.NrOfPoints(); i++)
  {
    std::vector<int> vecElements = ElementSet().ElementsAt(element.Point(i));
    if(vecElements.size() == 0)
      return false;

    values[i] = PointValue(element.Point(i), vecElements, WT_NONE, cb);
  }

  return true;
}

// map values from this valueset (and its elementset) to the given valueset (and its elementset)
void IValueSet::MapValues(IValueSet &TargetValues, TWeightingType nWeightingType, IParallelInitializationCallback *cb, IProgressBase *pProgress /* = 0 */) const
{
  const IElementSet &stTarget = TargetValues.ElementSet();

  if(pProgress) pProgress->AddSteps(stTarget.ElementSize());

  for(int i = 0; i < stTarget.ElementSize(); i++)
  {
    const IElement &el = stTarget.Element(i);
    std::vector<CValue> values;
    const IBody *pBody = dynamic_cast<const IBody *> (&el);
    assert(pBody != 0); // can only map for bodies
    MapValues(*pBody, values, nWeightingType, cb);
    TargetValues.PushBack(values);
    if(pProgress) pProgress->Step();
  }
}



}
