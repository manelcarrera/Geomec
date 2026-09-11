#ifndef _SavePropertyTensor_h_
#define _SavePropertyTensor_h_

#include "ITensorGroup.h"
#include "ModelBase.h"
#include "RGInterface.h"
#include "RGUtils.h"
#include "RockMechProcessor.h"
#include "SavePropertyBase.h"

namespace GeomecRGI {

class CSavePropertyTensor : public CSavePropertyBase {
public:
  CSavePropertyTensor(const RGProperty &rgProperty,
                      const ITensorGroup::CComponentComposite::TENSOR_COMPONENT &tensorComponent);
  virtual ~CSavePropertyTensor();

protected:
  template <typename Tensor>
  bool saveProperty(const Tensor &tensor, RGInterface &rgi, CModelBase &modelBase, const CRockMechProcessor &rmp,
                    double factor);

private:
  CSavePropertyTensor(const CSavePropertyTensor &rhs);
  CSavePropertyTensor &operator=(const CSavePropertyTensor &rhs);

  const ITensorGroup::CComponentComposite::TENSOR_COMPONENT m_tensorComponent;
};

// protected

namespace {

const QString TENSOR_COMPONENT_NOT_FOUND = "component %1 not found for result %2 and depletion stage %3";

} // anonymous namespace

template <typename Tensor>
bool CSavePropertyTensor::saveProperty(const Tensor &tensor, RGInterface &rgi, CModelBase &modelBase,
                                       const CRockMechProcessor &rmp, double factor) {
  CDepletionStage &stage =
      modelBase.DepletionStageEntry().StageByIndex(rgi.getCurrentDepletionStage().getDepletionStage());
  const IResultComponent *resultComponent =
      tensor.Components().ResultComponent(stage, CAnalysisType::AT_NONLIN, 0, m_tensorComponent);

  if (resultComponent == 0) {
    QString logLine = TENSOR_COMPONENT_NOT_FOUND.arg(m_tensorComponent).arg(tensor.Name()).arg(stage.Index());

    rmp.AddLogLine(logLine, &rgi, false, true);

    return false;
  }

  std::vector<double> values(modelBase.Mesh().Mesh().ElementSize());

  for (int i = 0; i < modelBase.Mesh().Mesh().ElementSize(); ++i) {
    const geo::IElement &element = modelBase.Mesh().Mesh().Element(i);
    geo::CValue value = resultComponent->ValuePoint(element.MidPoint());

    values[i] = (value.Valid() ? (factor * value.Value()) : RGUtils::nullReal());
  }

  rgi.saveProperty(m_RGProperty, values);

  return true;
}

} // namespace GeomecRGI

#endif // _SavePropertyTensor_h_
