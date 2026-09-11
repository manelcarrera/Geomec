#ifndef _SavePropertyTemplate_h_
#define _SavePropertyTemplate_h_

#include "FormationBase.h"
#include "ModelBase.h"
#include "RGInterface.h"
#include "RGUtils.h"
#include "SavePropertyBase.h"

namespace GeomecRGI {

template <unsigned int VALUETYPE> class CSavePropertyTemplate : public CSavePropertyBase {
public:
  CSavePropertyTemplate(const RGProperty &rgProperty);
  virtual ~CSavePropertyTemplate();

  virtual bool saveProperty(RGInterface &rgi, CModelBase &modelBase, const CRockMechProcessor &rmp);

protected:
  void saveProperty(RGInterface &rgi, CModelBase &modelBase, std::vector<double> &values,
                    const CRockMechProcessor &rmp);

private:
  CSavePropertyTemplate(const CSavePropertyTemplate &rhs);
  CSavePropertyTemplate &operator=(const CSavePropertyTemplate &rhs);
};

template <unsigned int VALUETYPE>
CSavePropertyTemplate<VALUETYPE>::CSavePropertyTemplate(const RGProperty &rgProperty) : CSavePropertyBase(rgProperty) {}

template <unsigned int VALUETYPE> CSavePropertyTemplate<VALUETYPE>::~CSavePropertyTemplate() {}

template <unsigned int VALUETYPE>
bool CSavePropertyTemplate<VALUETYPE>::saveProperty(RGInterface &rgi, CModelBase &modelBase,
                                                    const CRockMechProcessor &rmp) {
  std::vector<double> values(modelBase.Mesh().Mesh().ElementSize());

  saveProperty(rgi, modelBase, values, rmp);
  rgi.saveProperty(m_RGProperty, values);

  return true;
}

// protected

template <unsigned int VALUETYPE>
void CSavePropertyTemplate<VALUETYPE>::saveProperty(RGInterface &rgi, CModelBase &modelBase,
                                                    std::vector<double> &values, const CRockMechProcessor & /*rmp*/) {
  CDepletionStage &depletionStage =
      modelBase.DepletionStageEntry().StageByIndex(rgi.getCurrentDepletionStage().getDepletionStage());

  std::fill(values.begin(), values.end(), RGUtils::nullReal());

  for (int i = 0; i < modelBase.Mesh().Mesh().ElementSize(); ++i) {
    const geo::IElement &element = modelBase.Mesh().Mesh().Element(i);
    const CFormationBase *formationBase = modelBase.Mesh().Formation(element);

    if (formationBase) {
      const CFFMaterial &cffMaterial = formationBase->Material(depletionStage).Material(element);

      if (cffMaterial.IsParameter(VALUETYPE)) {
        values[i] = cffMaterial.ParameterValue(VALUETYPE);
      }
    }
  }
}

} // namespace GeomecRGI

#endif // _SavePropertyTemplate_h_
