#ifndef _cora_FrictionAngleValueTypeWrapper_h_
#define _cora_FrictionAngleValueTypeWrapper_h_

#include "IValueTypeWrapper.h"

namespace cora {

template <typename ParametersNode> class CFrictionAngleValueTypeWrapper : public IValueTypeWrapper {
public:
  CFrictionAngleValueTypeWrapper(const ParametersNode &parametersNode);
  virtual ~CFrictionAngleValueTypeWrapper();

  virtual bool hasDistributedValueType() const;
  virtual const std::vector<double> getDistributedValueType(const geo::IElement &element,
                                                            const CQuantity::UNIT unit) const;
  virtual const CDoubleQuantity &getDistributedValueTypeQuantity() const;

private:
  CFrictionAngleValueTypeWrapper(const CFrictionAngleValueTypeWrapper &rhs);
  CFrictionAngleValueTypeWrapper &operator=(const CFrictionAngleValueTypeWrapper &rhs);

  const ParametersNode &m_parametersNode;
};

template <typename ParametersNode>
CFrictionAngleValueTypeWrapper<ParametersNode>::CFrictionAngleValueTypeWrapper(const ParametersNode &parametersNode)
    : IValueTypeWrapper(), m_parametersNode(parametersNode) {}

template <typename ParametersNode> CFrictionAngleValueTypeWrapper<ParametersNode>::~CFrictionAngleValueTypeWrapper() {}

template <typename ParametersNode>
bool CFrictionAngleValueTypeWrapper<ParametersNode>::hasDistributedValueType() const {
  return m_parametersNode.DistributedFriction();
}

template <typename ParametersNode>
const std::vector<double>
CFrictionAngleValueTypeWrapper<ParametersNode>::getDistributedValueType(const geo::IElement &element,
                                                                        const CQuantity::UNIT unit) const {
  return m_parametersNode.FrictionAngle(element, unit);
}

template <typename ParametersNode>
const CDoubleQuantity &CFrictionAngleValueTypeWrapper<ParametersNode>::getDistributedValueTypeQuantity() const {
  return m_parametersNode.FrictionAngle();
}

} // namespace cora

#endif // _cora_FrictionAngleValueTypeWrapper_h_
