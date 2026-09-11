
#include "SavePropertyPorosity.h"
#include "ModelBase.h"
#include "ResultTree.h"
#include "RockMechProcessor.h"
#include "RGInterface.h"
#include "FormationBase.h"
#include "RGUtils.h"

namespace GeomecRGI
{

CSavePropertyPorosity::CSavePropertyPorosity(const RGProperty& rgProperty)
: CSavePropertyBase(rgProperty)
{
}

CSavePropertyPorosity::~CSavePropertyPorosity()
{
}

bool CSavePropertyPorosity::saveProperty(RGInterface& rgi,
  CModelBase& modelBase, const CRockMechProcessor& rmp)
{
  CDepletionStage& stage = modelBase.DepletionStageEntry().
  StageByIndex(rgi.getCurrentDepletionStage().getDepletionStage());
  const IResultComponent* pComp = modelBase.ResultTree().Porosity().
  ResultComponent(stage, CAnalysisType::AT_NONLIN, 0);

  if ((pComp == 0) && !stage.IsMarkedAsInitial())
  {
  QString l = QString("No porosity component found for depletion stage %1").
      arg(stage.Index());
  rmp.AddLogLine(l, &rgi, false, true);
  return false;
  }

  std::vector <double>
  vcValues(modelBase.Mesh().Mesh().ElementSize(), RGUtils::nullReal());
  bool logLineAdded = false;

  for (int i = 0; i < modelBase.Mesh().Mesh().ElementSize(); ++i)
  {
  const geo::IElement& elm = modelBase.Mesh().Mesh().Element(i);

  // Porosity not defined for faults; specifically, we shouldn't add an error message for interface elements
  if (elm.IsInterfaceElement())
      continue;

  if (pComp == 0)
  {
      const CFormationBase* formationBase = modelBase.Mesh().Formation(elm);

      if (formationBase)
      {
    const CFFMaterial& cffMaterial =
          formationBase->Material(stage).Material(elm);

    if (cffMaterial.IsParameter(IDT_VALUETYPE_POROSITY))
    {
          vcValues[i] = cffMaterial.ParameterValue(IDT_VALUETYPE_POROSITY);
    }
      }
  }
  else  // (pComp != 0)
  {
      geo::CValue val = pComp->ValuePoint(elm.MidPoint());

      if (!val.Valid())
      {
    if (!logLineAdded)
    {
          QString l =
      QString("Found an invalid porosity for depletion stage %1").
              arg(stage.Index());
          rmp.AddLogLine(l, &rgi, false, true);
          logLineAdded = true;
    }
      }
      else
      {
    vcValues[i] = val.Value();
      }
  }
  }

  rgi.saveProperty(m_RGProperty, vcValues);

  return true;
}

} // namespace GeomecRGI
