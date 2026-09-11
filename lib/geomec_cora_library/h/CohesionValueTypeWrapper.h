#ifndef _cora_CohesionValueTypeWrapper_h_
#define _cora_CohesionValueTypeWrapper_h_

#include "IValueTypeWrapper.h"

namespace cora
{

template <typename ParametersNode>
  class CCohesionValueTypeWrapper : public IValueTypeWrapper
{
  public:
    CCohesionValueTypeWrapper(const ParametersNode& parametersNode);
    virtual ~CCohesionValueTypeWrapper();

    virtual bool hasDistributedValueType() const;
    virtual const std::vector <double> getDistributedValueType(
      const geo::IElement& element, const CQuantity::UNIT unit) const;
    virtual const CDoubleQuantity& getDistributedValueTypeQuantity() const;
    
  private:
    CCohesionValueTypeWrapper(const CCohesionValueTypeWrapper& rhs);
    CCohesionValueTypeWrapper& operator = (
      const CCohesionValueTypeWrapper& rhs);

    const ParametersNode& m_parametersNode;
};

template <typename ParametersNode>
  CCohesionValueTypeWrapper <ParametersNode> ::CCohesionValueTypeWrapper(
    const ParametersNode& parametersNode)
  : IValueTypeWrapper()
  , m_parametersNode(parametersNode)
{
}

template <typename ParametersNode>
  CCohesionValueTypeWrapper <ParametersNode> ::~CCohesionValueTypeWrapper()
{
}

template <typename ParametersNode>
  bool CCohesionValueTypeWrapper <ParametersNode> ::hasDistributedValueType()
    const
{
  return m_parametersNode.DistributedCohesion();
}

template <typename ParametersNode>
  const std::vector <double> CCohesionValueTypeWrapper <ParametersNode> ::
    getDistributedValueType(const geo::IElement& element,
      const CQuantity::UNIT unit) const
{
  return m_parametersNode.Cohesion(element, unit);
}

template <typename ParametersNode>
  const CDoubleQuantity& CCohesionValueTypeWrapper <ParametersNode> ::
    getDistributedValueTypeQuantity() const
{
  return m_parametersNode.Cohesion();
}

} // namespace cora

#endif  // _cora_CohesionValueTypeWrapper_h_
