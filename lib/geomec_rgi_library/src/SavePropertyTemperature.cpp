
#include "SavePropertyTemperature.h"
#include "FormationBase.h"
#include "MeshBase.h"
#include "ModelBase.h"
#include "RGInterface.h"
#include "RGUtils.h"

namespace GeomecRGI {

CSavePropertyTemperature::CSavePropertyTemperature(const RGProperty &rgProperty) : CSavePropertyBase(rgProperty) {}

CSavePropertyTemperature::~CSavePropertyTemperature() {}

bool CSavePropertyTemperature::saveProperty(RGInterface &rgi, CModelBase &modelBase,
                                            const CRockMechProcessor & /*rmp*/) {
  CDepletionStage &stage =
      modelBase.DepletionStageEntry().StageByIndex(rgi.getCurrentDepletionStage().getDepletionStage());

  std::vector<double> vcValues(modelBase.Mesh().Mesh().ElementSize());
  int i;
  for (i = 0; i < modelBase.Mesh().Mesh().ElementSize(); ++i) {
    const geo::IElement &elm = modelBase.Mesh().Mesh().Element(i);
    IValueDomainScalar::TValueVec vcElementValues;
    const CFormationBase *pFormation = modelBase.Mesh().Formation(elm);
    double dResult = RGUtils::nullReal();
    if (pFormation) {
      vcElementValues = pFormation->EffectiveTemperature(stage).Component().ScalarData().ValueElement(elm);

      bool bValid = !vcElementValues.empty();
      double dSum = 0;
      for (size_t j = 0; j < vcElementValues.size(); ++j) {
        if (!vcElementValues[j].Valid()) {
          bValid = false;
          break;
        }
        dSum += vcElementValues[j].Value();
      }
      if (bValid) {
        dResult = 273.15 + dSum / vcElementValues.size(); // C -> K
      }
    }

    vcValues[i] = dResult;
  }

  rgi.saveProperty(m_RGProperty, vcValues);

  return true;
}

} // namespace GeomecRGI
