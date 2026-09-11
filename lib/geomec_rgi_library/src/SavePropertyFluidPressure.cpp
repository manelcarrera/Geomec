
#include "SavePropertyFluidPressure.h"
#include "ModelBase.h"
#include "MeshBase.h"
#include "FormationBase.h"
#include "RGInterface.h"
#include "Pressure.h"
#include "HorizonBase.h"
#include "FaultPressure.h"
#include "RGUtils.h"

namespace GeomecRGI
{

CSavePropertyFluidPressure::CSavePropertyFluidPressure(
  const RGProperty& rgProperty)
: CSavePropertyBase(rgProperty)
{
}

CSavePropertyFluidPressure::~CSavePropertyFluidPressure()
{
}

bool CSavePropertyFluidPressure::saveProperty(RGInterface& rgi,
  CModelBase& modelBase, const CRockMechProcessor& /*rmp*/)
{
  CDepletionStage& stage = modelBase.DepletionStageEntry().StageByIndex(rgi.getCurrentDepletionStage().getDepletionStage());

  std::vector<double> vcValues(modelBase.Mesh().Mesh().ElementSize());
  int i;
  for(i = 0; i < modelBase.Mesh().Mesh().ElementSize(); ++i)
  {
  const geo::IElement& elm = modelBase.Mesh().Mesh().Element(i);
  int nNod = elm.NrOfNodes();
  IValueDomainScalar::TValueVec vcElementValues;
  const CFormationBase* pFormation = modelBase.Mesh().Formation(elm);
  if(pFormation)
  {
      vcElementValues = pFormation->Pressure(stage).Component().ScalarData().ValueElement(elm);
  }
  else
  {
      const CHorizonBase* pFault = modelBase.Mesh().SlipHorizon(elm);
      if(pFault && pFault->Slip())
      {
    vcElementValues = pFault->Pressure(stage).Component().ScalarData().ValueElement(elm);
      }
      else
      {
    vcElementValues.resize(nNod, 0);
      }
  }

  double dSum = 0;
  bool bValid = true;
  for(size_t j = 0; j < vcElementValues.size(); ++j)
  {
      if(!vcElementValues[j].Valid())
      {
    bValid = false;
    break;
      }
      dSum += vcElementValues[j].Value() * 1e6; // MPa -> Pa
  }
  if(bValid && !vcElementValues.empty())
  {
      dSum /= vcElementValues.size();
  }
  else
      dSum = RGUtils::nullReal();

  vcValues[i] = dSum;
  }

  rgi.saveProperty(m_RGProperty, vcValues);

  return true;
}

} // namespace GeomecRGI
