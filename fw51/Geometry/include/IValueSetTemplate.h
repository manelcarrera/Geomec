/* Confidential Source Code Copyright (c) 2011 TNO DIANA BV                              Confidential */
 /*                                         Copyright (c) 2010 TNO DIANA BV */
#if !defined(_VALUESET_TEMPLATE__INCLUDED_)
#define _VALUESET_TEMPLATE__INCLUDED_

#include <vector>
#include <map>
#include <valarray>
#include "IElementSet.h"
#include "IBody.h"
#include "BodyQuadrilateral.h"
#include "BodyTriangle.h"
#include "EmbeddedTriangle.h"
#include "TiedPointBodyTriangle.h"
#include "IFace.h"

#include "GeometryExports.h"

namespace geo {

class IBody;
class IPoint;
class IElement;
class IElementSet;
class IParallelInitializationCallback;

template<class VALUE_TYPE>
class IValueSetTemplate
{
public:
  // typedef for a vector of values of one element
  typedef std::vector<VALUE_TYPE> TValueVec;

  IValueSetTemplate() {}
  virtual ~IValueSetTemplate() {}

  /*!
    get the elementset this valueset belongs to (get the size there as well)
  */
  virtual const IElementSet &ElementSet() const = 0;

  /*!
    get the element values for the element with the given index in the elementset
  */
  virtual void ElementValues(TValueVec& values, int iElementIndex, IParallelInitializationCallback *cb) const = 0;

  /*!
    check for emptyness
  */
  virtual bool IsEmpty() const = 0;

  /*!
    returns true when a VALUE_TYPE is not valid
  */
  virtual bool IsValid(const VALUE_TYPE& value) const = 0;

  // get nodal values for the element from smoothed values in the set
//virtual bool GetSmoothedNodalValues(const IElement &element, std::vector<CValue> &values) const;

  /*!
    map values on the known element set
  */
  void ElementValue(const IElement& element, TValueVec& element_values, IParallelInitializationCallback *cb) const;

  /*!
    returns the interpolated value by using the
  */
  VALUE_TYPE ValuePoint(const IPoint& point, IParallelInitializationCallback *cb) const;
protected:
  VALUE_TYPE InterpolateValue(const IPoint &point, int nElement, IParallelInitializationCallback *cb) const;
  bool Contains(const geo::IElement& target, int nSourceIndex) const;
  int ValueSize( int nElement ) const;
  virtual void FilterElements(std::vector<int> &indices) const;
};

template<class VALUE_TYPE>
VALUE_TYPE IValueSetTemplate<VALUE_TYPE>::InterpolateValue(const IPoint &point, int nElement, IParallelInitializationCallback *cb) const
{
  const geo::IElement& element = ElementSet().Element( nElement );
  TValueVec values( ValueSize( nElement ) );
  ElementValues( values, nElement, cb );

  for(size_t i = 0; i < values.size(); i++) {
    if(!IsValid(values[i])) return VALUE_TYPE();
  }

  IElement::TDoubleVec vcShape = element.IsoShapeFunction(point);
  assert( vcShape.size() == values.size() );

  VALUE_TYPE ret = values[0] * vcShape[0];
  for(size_t i = 1; i < values.size(); i++) {
    ret += values[i] * vcShape[i];
  }

  return ret;
}

template<class VALUE_TYPE>
void IValueSetTemplate<VALUE_TYPE>::FilterElements(std::vector<int> &/*indices*/) const
{
}

template<class VALUE_TYPE>
VALUE_TYPE IValueSetTemplate<VALUE_TYPE>::ValuePoint(const IPoint& point, IParallelInitializationCallback *cb) const
{
  // Try to find element
  std::vector<int> vcIndex = ElementSet().ElementsAt(point);
  FilterElements(vcIndex);
  if(vcIndex.size() == 0 )
    return VALUE_TYPE();
  if(vcIndex.size() == 1) // For efficiency reasons...
    return InterpolateValue( point, vcIndex[0], cb);
  VALUE_TYPE valsum;
  int nValues = 0;
  for(size_t i = 0; i < vcIndex.size(); ++i) {
    VALUE_TYPE val = InterpolateValue( point, vcIndex[i], cb );
    if(!(val == VALUE_TYPE()))
    {
      if(valsum == VALUE_TYPE())
        valsum = val;
      else
        valsum += val;
      ++nValues;
    }
  }

  if(!nValues)
    return VALUE_TYPE();
  return valsum / nValues;
}

template<class VALUE_TYPE>
void IValueSetTemplate<VALUE_TYPE>::ElementValue(const IElement& element, TValueVec& element_values, IParallelInitializationCallback *cb) const
{
  assert((element.NrOfNodes() != 0) || (element.NrOfPoints() == element_values.size()));
  assert((element.NrOfNodes() == 0) || (element.NrOfNodes() == element_values.size()));

  // Do we deal with a mesh element
  if(&ElementSet() == element.IndexingElementSet()) {
    assert( element_values.size() == element.NrOfPoints() );
    // Dealing with a element from mesh ...
    ElementValues(element_values, element.Index(), cb);
    return ;
  }

  // Do we deal with a face from a body of our mesh?
  const CTiedPointBodyTriangle *pTiedPointBodyTriangle = dynamic_cast<const CTiedPointBodyTriangle *>(&element);
  const CBodyTriangle* pBodyTriangle =
    dynamic_cast<const CBodyTriangle*> (&element);
  const CBodyQuadrilateral* pBodyQuadrilateral =
    dynamic_cast<const CBodyQuadrilateral*> (&element);
  const CEmbeddedTriangle* pEmbeddedTriangle =
    dynamic_cast<const CEmbeddedTriangle*> (&element);

  if(pEmbeddedTriangle) {
    if(pEmbeddedTriangle->Parent()->IndexingElementSet() == &ElementSet()) {
      // Retrieve values of the parent and checked them
      const geo::IElement& parent_element = *pEmbeddedTriangle->Parent();
      TValueVec values(parent_element.NrOfPoints());
      ElementValues( values, parent_element.Index(), cb );
      for(size_t i = 0; i < values.size(); i++) {
        if(!IsValid(values[i])) return;
      }
      
      for(int nPoint = 0; nPoint < element.NrOfPoints(); nPoint++) {
        IElement::TDoubleVec vcShape = parent_element.IsoShapeFunction(element.Point(nPoint));
        assert( vcShape.size() == values.size() );

        element_values[nPoint] = values[0] * vcShape[0];
        for(size_t i = 1; i < values.size(); i++) {
          element_values[nPoint] += values[i] * vcShape[i];
        }
      }
      return;  // Ready
    }
  }

  if (pTiedPointBodyTriangle)
  {
    geo::CPoint point;
    const geo::IInterfaceElement *ifElt = nullptr;
    int offset = 0;

    for (int i = 0; i < 3; ++i)
    {
      ifElt = pTiedPointBodyTriangle->GetTying(i, point, offset);

      if (ifElt)
      {
        TValueVec values(ifElt->NrOfPoints());
        ElementValues(values, ifElt->Index(), cb);
        geo::IElement::TDoubleVec vcShape = ifElt->IsoShapeFunction(point);

        for (size_t j = 0; j < 3; j++) {
          if (!IsValid(values[j + offset])) return;
        }

        element_values[i] = values[offset] * vcShape[offset] + values[offset + 1] * vcShape[offset + 1] + values[offset + 2] * vcShape[offset + 2];
      }
    }
    return;
  }

  if(pBodyTriangle || pBodyQuadrilateral) {
    const IBody *pBody;
    if(pBodyTriangle) pBody = pBodyTriangle->Parent();
    else              pBody = pBodyQuadrilateral->Parent();

    if(pBody->IndexingElementSet() == &ElementSet()) {
      TValueVec body_values( pBody->NrOfPoints() );
      ElementValues(body_values, pBody->Index(), cb);
      if(pBodyTriangle) {
        for(int i = 0; i < element_values.size(); i++)
          element_values[i] = body_values[ pBodyTriangle->BodyNode(i) ];
      } else {
        for(int i = 0; i < element_values.size(); i++)
          element_values[i] = body_values[ pBodyQuadrilateral->BodyNode(i) ];
      }
      return;
    }
  }

  // Not a mesh element. Is the target element embedded in a source element?
  std::vector<int> vcElement = ElementSet().ElementsAt(element.Point(0));
  for(size_t i = 0; i < vcElement.size(); i++) {
    // Test: are all the points in the element
    if( Contains(element, vcElement[i]) ) {
      for(int j = 0; j < element_values.size(); j++) {
        element_values[j] = InterpolateValue( element.Point(j), vcElement[i], cb );
      }
      return;
    }
  }

  // Do simple point interpolation
  for(size_t i = 0; i < element_values.size(); i++) {
    element_values[i] = ValuePoint( element.Point((int)i), cb );
  }
}

template<class VALUE_TYPE>
int IValueSetTemplate<VALUE_TYPE>::ValueSize( int nElement ) const
{
  const geo::IElement& element = ElementSet().Element( nElement );
  if( !element.NrOfNodes() ) return element.NrOfPoints();
  return (int)element.NrOfNodes();
}

template<class VALUE_TYPE>
bool IValueSetTemplate<VALUE_TYPE>::Contains(const geo::IElement& target, int nSourceIndex) const
{
  const geo::IElement& source_element = ElementSet().Element(nSourceIndex);
  for(int i = 0; i < target.NrOfPoints(); i++) {
    if( !source_element.Contains( target.Point(i), true) ) return false;
  }
  return true;
}

}

#endif // !defined(_VALUESET_TEMPLATE__INCLUDED_)
