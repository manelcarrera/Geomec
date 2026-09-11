#ifndef _cora_ValueTypeWrapper_h_
#define _cora_ValueTypeWrapper_h_

#include "IValueTypeWrapper.h"
#include "ValueTypes.h"
#include "CohesionValueTypeWrapper.h"
#include "FrictionAngleValueTypeWrapper.h"

namespace cora
{

template <typename ParametersNode>
  class CValueTypeWrapper
{
  public:
    CValueTypeWrapper(const ParametersNode& parametersNode,
      unsigned int valueTypeID);
    ~CValueTypeWrapper();

    bool isValid() const;

    bool hasDistributedValueType() const;
    const std::vector <double> getDistributedValueType(
      const geo::IElement& element, const CQuantity::UNIT unit) const;
    const CDoubleQuantity& getDistributedValueTypeQuantity() const;
    
  private:
    CValueTypeWrapper(const CValueTypeWrapper& rhs);
    CValueTypeWrapper& operator = (const CValueTypeWrapper& rhs);

    static TValueTypeWrapper selectValueTypeWrapper(
      const ParametersNode& parametersNode, unsigned int valueTypeID);

    TValueTypeWrapper m_valueTypeWrapper;
};

template <typename ParametersNode>
  CValueTypeWrapper <ParametersNode> ::CValueTypeWrapper(
    const ParametersNode& parametersNode, unsigned int valueTypeID)
  : m_valueTypeWrapper(selectValueTypeWrapper(parametersNode, valueTypeID))
{
}

template <typename ParametersNode>
  CValueTypeWrapper <ParametersNode> ::~CValueTypeWrapper()
{
}

template <typename ParametersNode>
  bool CValueTypeWrapper <ParametersNode> ::isValid() const
{
  return (m_valueTypeWrapper != 0);
}

template <typename ParametersNode>
  bool CValueTypeWrapper <ParametersNode> ::hasDistributedValueType() const
{
  return m_valueTypeWrapper->hasDistributedValueType();
}

template <typename ParametersNode>
  const std::vector <double> CValueTypeWrapper <ParametersNode> ::getDistributedValueType(
    const geo::IElement& element, const CQuantity::UNIT unit) const
{
  return m_valueTypeWrapper->getDistributedValueType(element, unit);
}

template <typename ParametersNode>
  const CDoubleQuantity& CValueTypeWrapper <ParametersNode> ::getDistributedValueTypeQuantity()
    const
{
  return m_valueTypeWrapper->getDistributedValueTypeQuantity();
}

// private

// static

template <typename ParametersNode>
  TValueTypeWrapper CValueTypeWrapper <ParametersNode> ::selectValueTypeWrapper(
    const ParametersNode& parametersNode, unsigned int valueTypeID)
{
  TValueTypeWrapper valueTypeWrapper(0);

  if (valueTypeID == IDT_VALUETYPE_COHESION)
  {
    valueTypeWrapper = TValueTypeWrapper(
      new CCohesionValueTypeWrapper <ParametersNode> (parametersNode));
  }
  else if (valueTypeID == IDT_VALUETYPE_FRICTION_ANGLE)
  {
    valueTypeWrapper = TValueTypeWrapper(
      new CFrictionAngleValueTypeWrapper <ParametersNode> (parametersNode));
  }
  else
  {
    assert(false);
  }

  return valueTypeWrapper;
}

} // namespace cora

#endif  // _cora_ValueTypeWrapper_h_
