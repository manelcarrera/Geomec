#ifndef _cora_ObjectParametersNodeValue_h_
#define _cora_ObjectParametersNodeValue_h_

#include <limits>

#ifdef _WIN32
#define MAKESTRING2(str)  #str
#define MAKESTRING(str) MAKESTRING2(str)
#pragma message("WARNING: disable C4800 (bool to int in Qt) in " __FILE__ "[" MAKESTRING(__LINE__) "]")
#pragma warning(push)
#pragma warning(disable: 4800)
#endif

#include "InterfaceElement.h"
#include "ValueTypeWrapper.h"
#include "ElementGroup.h"
#include "HorizonBase.h"
#include "NonMeshedSurfaceParametersNode.h"
#include "Utilities4ValueVector.h"
#include "GetValueTypeInfo.h"

namespace cora
{

template <typename Surface, typename ParametersNode>
  class CObjectParametersNodeValue
{
  public:
    CObjectParametersNodeValue(const Surface* surface, unsigned int valueTypeID,
      const ParametersNode& parametersNode);

    double getMean() const;
    std::pair <geo::CValue, geo::CValue> getRange() const;

  private:
    CObjectParametersNodeValue(const CObjectParametersNodeValue& rhs);
    CObjectParametersNodeValue& operator = (CObjectParametersNodeValue rhs);

    double calculateArea() const;
    double getArea(const geo::IElement& element) const;

    const Surface* m_surface;
    unsigned int m_valueTypeID;
    const ParametersNode& m_parametersNode;
};

template <typename Surface, typename ParametersNode>
  CObjectParametersNodeValue <Surface, ParametersNode>::
    CObjectParametersNodeValue(const Surface* surface, unsigned int valueTypeID,
    const ParametersNode& parametersNode)
: m_surface(surface)
, m_valueTypeID(valueTypeID)
, m_parametersNode(parametersNode)
{
}

template <typename Surface, typename ParametersNode>
  double CObjectParametersNodeValue <Surface, ParametersNode>::getMean() const
{
  double totalArea = calculateArea();
  double mean = 0;

  CValueTypeWrapper <ParametersNode>
    valueTypeWrapper(m_parametersNode, m_valueTypeID);

  if (valueTypeWrapper.isValid())
  {
    if (valueTypeWrapper.hasDistributedValueType())
    {
      for (int element = 0; element < m_surface->ElementSize(); ++element)
      {
        const geo::IElement& geoElement = m_surface->Element(element);
        std::vector <double> valueType = valueTypeWrapper.
          getDistributedValueType(geoElement, CDoubleQuantity::SI_UNIT);
        size_t count = 0;
        double total = 0;

        for (std::vector <double> ::const_iterator d = valueType.begin();
          d != valueType.end(); ++d)
        {
          ++count;
          total += *d;
        }

        double area = getArea(geoElement);

        mean += (area / totalArea) * (total / (count == 0 ? 1 : count));
      }
    }
    else
    {
      const CDoubleQuantity&
        quantity = valueTypeWrapper.getDistributedValueTypeQuantity();

      mean = quantity.Value();
    }
  }

  return mean;
}

template <typename Surface, typename ParametersNode>
  std::pair <geo::CValue, geo::CValue>
    CObjectParametersNodeValue <Surface, ParametersNode>::getRange() const
{
  CGetValueTypeInfo& getValueTypeInfo(CGetValueTypeInfo::instance(
    const_cast <CModelBase*> (dynamic_cast <const CModelBase*> (
      &m_parametersNode.Model()))));
  std::pair <geo::CValue, geo::CValue> range = getValueTypeInfo.
    getRange(m_valueTypeID, getValueTypeInfo.getImportTag(m_valueTypeID));

  CValueTypeWrapper <ParametersNode>
    valueTypeWrapper(m_parametersNode, m_valueTypeID);

  if (valueTypeWrapper.isValid())
  {
    if (valueTypeWrapper.hasDistributedValueType())
    {
      range = std::make_pair(std::numeric_limits <double> ::max(),
        -std::numeric_limits <double> ::max());

      for (int element = 0; element < m_surface->ElementSize(); ++element)
      {
        const geo::IElement& geoElement = m_surface->Element(element);
        std::vector <double> valueType = valueTypeWrapper.
          getDistributedValueType(geoElement, CDoubleQuantity::SI_UNIT);

        range.first = std::min(range.first.Value(),
          CUtilities4ValueVector::calculateAverage(valueType));
        range.second = std::max(range.second.Value(),
          CUtilities4ValueVector::calculateAverage(valueType));
      }
    }
    else
    {
      const CDoubleQuantity&
        quantity = valueTypeWrapper.getDistributedValueTypeQuantity();

      range.first = quantity.Value();
      range.second = quantity.Value();
    }
  }

  return range;
}

// private

template <typename Surface, typename ParametersNode>
  double CObjectParametersNodeValue <Surface, ParametersNode>::calculateArea()
    const
{
  double totalArea = 0;

  for (int element = 0; element < m_surface->ElementSize(); ++element)
  {
    totalArea += getArea(m_surface->Element(element));
  }

  return totalArea;
}

template <typename Surface, typename ParametersNode>
  double CObjectParametersNodeValue <Surface, ParametersNode>::getArea(
    const geo::IElement& element) const
{
  assert(false);

  return 0;
}

// forward declaration of full specializations

template <>
  double CObjectParametersNodeValue <geo::CElementGroup,
    CFaultParametersNode>::getArea(const geo::IElement& element) const;

template <>
  double CObjectParametersNodeValue <geo::IElementSet,
    CNonMeshedSurfaceParametersNode>::getArea(const geo::IElement& element)
      const;

} // namespace cora

#ifdef _WIN32
#pragma warning(pop)
#endif

#endif  // _cora_ObjectParametersNodeValue_h_
