
#include "SavePropertyPermMultFrac.h"
#include "ModelBase.h"
#include "MeshBase.h"
#include "RGInterface.h"
#include "FormationBase.h"
#include "MaterialFractureApertureBase.h"
#include "ResultTree.h"
#include "RockMechProcessor.h"

namespace GeomecRGI
{

CSavePropertyPermMultFrac::CSavePropertyPermMultFrac(const RGProperty& rgProperty,
  TDensityDirection densityDirection)
: CSavePropertyBase(rgProperty)
, m_DensityDirection(densityDirection)
{
}

CSavePropertyPermMultFrac::~CSavePropertyPermMultFrac()
{
}

bool CSavePropertyPermMultFrac::saveProperty(RGInterface& rgi,
  CModelBase& modelBase, const CRockMechProcessor& rmp)
{
  std::vector <double> values(modelBase.Mesh().Mesh().ElementSize());
  CDepletionStage& depletionStage =
    modelBase.DepletionStageEntry().StageByIndex(
      rgi.getCurrentDepletionStage().getDepletionStage());
  const IScalarResultComponent* scalarResultComponent =
    getScalarResultComponent(modelBase, depletionStage);
  bool valuesSeen = false;

  for (int i = 0; i < modelBase.Mesh().Mesh().ElementSize(); ++i)
  {
    const geo::IElement& element = modelBase.Mesh().Mesh().Element(i);
    const CFormationBase* formationBase = modelBase.Mesh().Formation(element);
    std::pair <double, bool> length = std::make_pair(1, true);

    if (formationBase)
    {
      try
      {
        const CFFMaterial& cffMaterial =
          formationBase->Material(depletionStage).Material(element);
        /*const CMaterialFractureApertureBase& materialFractureApertureBase =*/
          dynamic_cast <const CMaterialFractureApertureBase&> (
            cffMaterial.Material());

        if (scalarResultComponent)
        {
          length = getLength(scalarResultComponent, modelBase, element);

          if (length.second)
          {
            valuesSeen = true;
          }
        }
      }

      catch (const std::bad_cast&)
      {
      }
    }

    assert(length.second);

    values[i] = length.first;
  }

  rgi.saveProperty(m_RGProperty, values);

  if (!valuesSeen)
  {
    if (scalarResultComponent != 0)
    {
      rmp.AddLogLine("No fracture aperture material has been found.");
    }
    else  // scalarResultComponent == 0
    {
      QString message = QString("No result could be found for property '%1' in "
        "combination with depletion stage '%2'.").
        arg(rmp.PropertyName(m_RGProperty).toStdString().c_str()).
        arg(depletionStage.Name());

      rmp.AddLogLine(message);
    }
  }

  return true;
}

// private

std::pair <double, bool> CSavePropertyPermMultFrac::getLength(
  const IScalarResultComponent* scalarResultComponent,
  const CModelBase& modelBase, const geo::IElement& element) const
{
  IValueDomainScalar::TValueVec values(element.NrOfPoints());

  scalarResultComponent->ElementValues(values, element, modelBase.Mesh().ValueUnit(), geo::IParallelInitializationCallback::Sequential);

  double sum = 0;
  bool valid = true;

  for(size_t i = 0; i < values.size(); ++i)
  {
    if (!values[i].Valid())
    {
      valid = false;

      break;
    }

    sum += values[i].Value();
  }

  if (valid && !values.empty())
  {
    sum /= values.size();
  }

  return std::make_pair(sum, valid);
}

const IScalarResultComponent* CSavePropertyPermMultFrac::
  getScalarResultComponent(CModelBase& modelBase,
    const CDepletionStage& depletionStage) const
{
  switch (m_DensityDirection)
  {
    case HIGH:
      return dynamic_cast <const IScalarResultComponent*> (
        modelBase.ResultTree().
          Aperture(CResultTree::CScalarResult::PERMEABILITY_MULTIPLIER_HI).
          ResultComponent(depletionStage, CAnalysisType(CAnalysisType::AT_NONLIN)));
    case INTERMEDIATE:
      return dynamic_cast <const IScalarResultComponent*> (
        modelBase.ResultTree().
          Aperture(CResultTree::CScalarResult::PERMEABILITY_MULTIPLIER_ME).
          ResultComponent(depletionStage, CAnalysisType(CAnalysisType::AT_NONLIN)));
    case LOW:
      return dynamic_cast <const IScalarResultComponent*> (
        modelBase.ResultTree().
          Aperture(CResultTree::CScalarResult::PERMEABILITY_MULTIPLIER_LO).
          ResultComponent(depletionStage, CAnalysisType(CAnalysisType::AT_NONLIN)));
    default:
      assert(false);
  }

  return 0;
}

geo::CVector CSavePropertyPermMultFrac::getVector(const CFFMaterial& cffMaterial,
  const CMaterialFractureApertureBase& materialFractureApertureBase) const
{
  switch (m_DensityDirection)
  {
    case HIGH:
      return materialFractureApertureBase.HighDensityDirection(cffMaterial);
    case INTERMEDIATE:
      return materialFractureApertureBase.
        IntermediateDensityDirection(cffMaterial);
    case LOW:
      return materialFractureApertureBase.LowDensityDirection(cffMaterial);
    default:
      assert(false);
  }

  return geo::CVector();
}

} // namespace GeomecRGI
