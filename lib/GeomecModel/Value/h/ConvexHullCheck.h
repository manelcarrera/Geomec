#ifndef _CONVEX_HULL_CHECK_
#define _CONVEX_HULL_CHECK_

// Copy of the convex hull check in FormationLoadComponent. TODO: (at appropriate time) replace code there with this

#include "IValueDomain.h"
#include "IElement.h"
#include "IPointSet.h"


// We use a wrapper, so that we can control what we send to the check routine (needed, eg, with Strains)
class IDistributedSetWrapper
{
public:
  virtual size_t DistributedSize() = 0;

  virtual const IPointSet& DistributedPointset(size_t index) = 0;
  virtual void DistributedValue(size_t index, const geo::IElement& elm, const CDoubleQuantity::UNIT unit, IValueDomainScalar::TValueVec& vcValues) = 0;
  virtual void DistributedValue(size_t index, const geo::IElement& elm, const CDoubleQuantity::UNIT unit, IValueDomainVector::TValueVec& vcValues) = 0;
  virtual void DistributedValue(size_t index, const geo::IElement& elm, const CDoubleQuantity::UNIT unit, IValueDomainTensor::TValueVec& vcValues) = 0;

  virtual bool DistributedOnly() = 0;
  virtual bool DoAveraging() = 0;
};


template<class VALUE_DOMAIN_TYPE>
class CConvexHullCheck
{
public:
  static void ValueElement(IDistributedSetWrapper& distriSet, const geo::IElement& elm, const CQuantity::UNIT unit, VALUE_DOMAIN_TYPE& vcRet);
};





template<class VALUE_DOMAIN_TYPE>
void CConvexHullCheck<VALUE_DOMAIN_TYPE>::ValueElement(IDistributedSetWrapper& distriSet, const geo::IElement& elm, const CQuantity::UNIT unit, VALUE_DOMAIN_TYPE& vcRet)
{
  vcRet.resize(elm.NrOfPoints());
  std::vector<int> vcNrOfValue(elm.NrOfPoints(), 0);

  if (distriSet.DistributedSize() > 0)
  {
    std::vector <double> distance(elm.NrOfPoints(), 0);
    bool aPointInConvexHull = false;

    // wjrx mantis 3480
    //
    // Drawing of 'Full Formations' and 'Formation Skin' was different
    // for mapped values from element sets.
    //
    // A 3d Hexa-element consists of sub elements of type Quad.
    // If elm is a Quad, we use the parent element.
    //
    // Similar action for Triangle.
    //
    // This works for 2d elements, because a 2d element is never an IBody.
    //
    const geo::IElement *pElement = dynamic_cast<const geo::IBody *> (&elm);

    const geo::CBodyTriangle *pBodyTriangle = dynamic_cast<const geo::CBodyTriangle *>(&elm);
    if(pBodyTriangle) pElement = pBodyTriangle->Parent();

    const geo::CBodyQuadrilateral *pBodyQuad = dynamic_cast<const geo::CBodyQuadrilateral *>(&elm);
    if (pBodyQuad) pElement = pBodyQuad->Parent();

    if (!pElement) pElement= &elm;

    for (size_t i = 0; i < distriSet.DistributedSize(); ++i)
    {
      const IPointSet& pointSet = distriSet.DistributedPointset(i);

      if (pointSet.PointInConvexHull(pElement->MidPoint()))
      {
        aPointInConvexHull = true;
        break;
      }
    }

    for (size_t i = 0; i < distriSet.DistributedSize(); ++i)
    {
      const IPointSet& pointSet = distriSet.DistributedPointset(i);

      VALUE_DOMAIN_TYPE vcValue;
      distriSet.DistributedValue(i, elm, unit, vcValue);
      
      bool anElementInConvexHull = pointSet.PointInConvexHull(pElement->MidPoint());

      for (int n = 0; n < elm.NrOfPoints(); n++)
      {
        if (aPointInConvexHull && vcValue[n].Valid())
        {
          if (anElementInConvexHull)
          {
            if (vcNrOfValue[n] == 0)
              vcRet[n] = vcValue[n];
            else
              vcRet[n] = vcRet[n] + vcValue[n];
            if (distriSet.DoAveraging())
              ++vcNrOfValue[n];
          }
        }
        else if (distriSet.DistributedOnly() && vcValue[n].Valid()) // validity necessary?
        {
          // wjrx mantis 3480
          //
          // formationload.PointSet() cal be a CPointSet or an IElementSet.
          //
          const CPointSet* pPointSet = dynamic_cast <const CPointSet*> (&pointSet);

          geo::CPoint midPoint;
          double representativeLength=-1;

          if (pPointSet)
          {
            // if we have a convex hull, use it to calc midPoint and representativeLength
            if (pPointSet->m_pHull_3D != 0)
            {
              midPoint= pPointSet->m_pHull_3D->MidPoint();
              representativeLength = pPointSet->m_pHull_3D->RepresentativeLength();
            }
            else if (pPointSet->m_pHull_2D != 0)
            {
              midPoint= pPointSet->m_pHull_2D->MidPoint();
              representativeLength= pPointSet->m_pHull_2D->RepresentativeLength();
            }
          }

          if (representativeLength < 0) // not CPointSet or no convex hull
          {
            midPoint = (pointSet.Min() + pointSet.Max()) / 2;

            // representativeLength is an approximation of the diameter of the point cloud
            //
            representativeLength = pointSet.Min().Distance(pointSet.Max()) / 2;
          }

          if (representativeLength > 0)
          {
            double val = midPoint.Distance(elm.Point(n)) / representativeLength;

            if (distance[n] == 0 || val < distance[n])
            {
                distance[n] = val;
                vcRet[n] = vcValue[n];
                vcNrOfValue[n] = 1;
            }
          }
        }
      }
    }

    for (size_t n = 0; n < elm.NrOfPoints(); ++n)
    {
      if(vcNrOfValue[n] > 1)
        vcRet[n] = vcRet[n] / vcNrOfValue[n];
    }
  }

}




#endif