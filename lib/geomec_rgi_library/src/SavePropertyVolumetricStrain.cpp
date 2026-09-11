
#include "SavePropertyVolumetricStrain.h"
#include "FormationBase.h"
#include "ModelBase.h"
#include "Pressure.h"
#include "RGInterface.h"
#include "RGUtils.h"
#include "ResultRegister.h"
#include "ResultTree.h"
#include "RockMechProcessor.h"

namespace GeomecRGI {

CSavePropertyVolumetricStrain::CSavePropertyVolumetricStrain(const RGProperty &rgProperty)
    : CSavePropertyBase(rgProperty) {}

CSavePropertyVolumetricStrain::~CSavePropertyVolumetricStrain() {}

bool CSavePropertyVolumetricStrain::saveProperty(RGInterface &rgi, CModelBase &modelBase,
                                                 const CRockMechProcessor &rmp) {
  CDepletionStage &stage =
      modelBase.DepletionStageEntry().StageByIndex(rgi.getCurrentDepletionStage().getDepletionStage());

  if (!modelBase.ResultRegister().ResultsAvailable(CAnalysisType::AT_NONLIN, stage)) {
    QString l;
    l = QString("No non-linear results available for depletion stage %1").arg(stage.Index());
    rmp.AddLogLine(l, &rgi, false, true);
    return false;
  }

  const CDepletionStage &iniStage = stage.InitialStage();

  const IValueComponentBase *pComp = modelBase.ResultTree().TotalStrain().Invariants().ResultComponent(
      stage, CAnalysisType::AT_NONLIN, 0, IStrainTensorGroup::CInvariantComposite::INV_I1);
  const IValueComponentBase *pIsoStressComp =
      modelBase.ResultTree().TotalStress().InvariantComposite()->ResultComponent(
          stage, CAnalysisType::AT_NONLIN, 0, IStressTensorGroup::CInvariantComposite::INV_I1);
  const IValueComponentBase *pIsoStressCompIni =
      modelBase.ResultTree().TotalStress().InvariantComposite()->ResultComponent(
          iniStage, CAnalysisType::AT_NONLIN, 0, IStressTensorGroup::CInvariantComposite::INV_I1);
  if (!pComp) {
    QString l;
    l = QString("No volumetric strains found for depletion stage %1").arg(stage.Index());
    rmp.AddLogLine(l, &rgi, false, true);
    return false;
  }
  assert(pComp->Defined());
  std::vector<double> vcValues(modelBase.Mesh().Mesh().ElementSize());
  int i;
  for (i = 0; i < modelBase.Mesh().Mesh().ElementSize(); ++i) {
    const geo::IElement &elm = modelBase.Mesh().Mesh().Element(i);
    IValueDomainScalar::TValueVec vcElementValues = pComp->ScalarData().ValueElement(elm);
    double dSum = 0;
    bool bValid = true;
    for (size_t j = 0; j < vcElementValues.size(); ++j) {
      if (!vcElementValues[j].Valid()) {
        bValid = false;
        break;
      }
      dSum += vcElementValues[j].Value();
    }
    if (bValid && !vcElementValues.empty()) {
      dSum /= vcElementValues.size();
      const CFormationBase *pFormation = modelBase.Mesh().Formation(elm);
      if (pFormation) {
        const CFFMaterial &ffmat = pFormation->Material(stage).Material(elm);
        if (ffmat.IsParameter(IDT_VALUETYPE_POROSITY) && ffmat.IsParameter(IDT_VALUETYPE_THERMAL_EXPANSION)) {
          double phi = ffmat.ParameterValue(IDT_VALUETYPE_POROSITY);
          double beta = ffmat.ParameterValue(IDT_VALUETYPE_THERMAL_EXPANSION);
          IValueDomainScalar::TValueVec vcT =
              pFormation->EffectiveTemperature(stage).Component().ScalarData().ValueElement(elm);
          IValueDomainScalar::TValueVec vcT0 =
              pFormation->EffectiveTemperature(iniStage).Component().ScalarData().ValueElement(elm);

          geo::CValue T = 0;
          geo::CValue T0 = 0;
          for (size_t j = 0; j < vcT.size(); ++j)
            T += vcT[j];
          T /= vcT.size();
          for (size_t j = 0; j < vcT0.size(); ++j)
            T0 += vcT0[j];
          T0 /= vcT0.size();
          if (T.Valid() && T0.Valid())
            dSum += beta * (1 - phi) * (T.Value() - T0.Value());

          if (ffmat.IsParameter(IDT_VALUETYPE_GRAINSTIFFNESS)) {
            double Kg = ffmat.ParameterValue(IDT_VALUETYPE_GRAINSTIFFNESS);
            if (Kg != 0) {
              geo::CValue P = 0;
              geo::CValue P0 = 0;
              IValueDomainScalar::TValueVec vcP =
                  pFormation->Pressure(stage).Component().ScalarData().ValueElement(elm);
              for (size_t j = 0; j < vcP.size(); ++j)
                P += vcP[j];
              P /= vcP.size();
              IValueDomainScalar::TValueVec vcP0 =
                  pFormation->Pressure(iniStage).Component().ScalarData().ValueElement(elm);
              for (size_t j = 0; j < vcP0.size(); ++j)
                P0 += vcP0[j];
              P0 /= vcP0.size();
              if (pIsoStressComp && pIsoStressCompIni) {
                IValueDomainScalar::TValueVec vcIsoStressValues = pIsoStressComp->ScalarData().ValueElement(elm);
                IValueDomainScalar::TValueVec vcIsoStressValuesIni = pIsoStressCompIni->ScalarData().ValueElement(elm);
                assert(vcIsoStressValues.size() == vcIsoStressValuesIni.size());
                double dStressSum = 0;
                bool bValid2 = true;
                for (size_t j = 0; j < vcElementValues.size(); ++j) {
                  if (!vcIsoStressValues[j].Valid() || !vcIsoStressValuesIni[j].Valid()) {
                    bValid2 = false;
                    break;
                  }
                  dStressSum += vcIsoStressValues[j].Value() - vcIsoStressValuesIni[j].Value();
                }

                if (bValid2 && !vcIsoStressValues.empty() && P.Valid() && P0.Valid()) {
                  dStressSum /= vcIsoStressValues.size();
                  dSum += -dStressSum / Kg + (phi / Kg) * (P.Value() - P0.Value());
                }
              }
            }
          }
        }
      }
    } else
      dSum = RGUtils::nullReal();
    vcValues[i] = bValid ? dSum : RGUtils::nullReal();
  }
  rgi.saveProperty(m_RGProperty, vcValues);

  return true;
}

} // namespace GeomecRGI
