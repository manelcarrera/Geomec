#include "stdafx.h"

#include "DumpModel.h"
#include "ModelBase.h"
#include "TetraModel.h"
#include "MeshBase.h"
#include "unitnode.h"
#include "DepletionStage.h"
#include "GlobalPressure.h"
#include "GlobalTemperature.h"
#include "GlobalInitialStressNode.h"
#include "Pressure.h"
#include "StrainLoad.h"
#include "TetraEntryTypes.h"
#include "TetraSubHorizon.h"
#include "GlobalUnitNode.h"
#include "FaultParametersNode.h"
#include "FaultPressure.h"
#include "FieldFactor.h"
#include "InterfaceBoundary.h"
#include "PressureSupportNode.h"
#include "DisplacementSupportNode.h"
#include "Upscaling.h"
#include "ZoominModelEntry.h"
#include "ZoominModelPlaceHolder.h"

CDumpModel::CDumpModel(const CModelBase& model)
: m_model(model)
, m_lineTypes()
, m_lines()
, m_vcStages()
, m_vcFormations()
, m_vcFaults()
{
  const CDepletionStage* pStage = &model.InitialDepletionStage();

  while (pStage)
  {
  m_vcStages.push_back(pStage);

  if (pStage->Last())
  {
      pStage = 0;
  }
  else
  {
      pStage = &pStage->Next();
  }
  }

  // formations
  TFormationBaseEntry& entry =
  (TFormationBaseEntry&) *model.GraphEntry(MD_BASE_FORMATION);
  TFormationBaseEntry::TNodeSet stNode = entry.EntryNodes();

  for (TFormationBaseEntry::TNodeSet::const_iterator it = stNode.begin();
  it != stNode.end(); it++)
  {
  m_vcFormations.push_back(*it);
  }

  // waij TFS 96130
  // faults
  CTetraSubHorizonEntry* pEntryF = 
  (CTetraSubHorizonEntry*)model.GraphEntry(MD_TETRA_SUB_FAULT);

  if(pEntryF)
  {
  CTetraSubHorizonEntry::TNodeSet stFaults = pEntryF->EntryNodes();
  for (CTetraSubHorizonEntry::TNodeSet::const_iterator itF = stFaults.begin();
      itF != stFaults.end(); ++itF)
  {
      m_vcFaults.push_back(*itF);
  }
  }
}

void CDumpModel::dump(const QString& modelName, const QString& fileLocation)
{
  assert(m_lineTypes.size() == m_lines.size());
  generalModelInfo(modelName, fileLocation);
  assert(m_lineTypes.size() == m_lines.size());
  nonLinCalcSettingsInfo();
  assert(m_lineTypes.size() == m_lines.size());
  analysisOptionsInfo();
  assert(m_lineTypes.size() == m_lines.size());
  globalDataInfo();
  assert(m_lineTypes.size() == m_lines.size());
  depletionStageInfo();
  assert(m_lineTypes.size() == m_lines.size());
  pressureDataInfo();
  assert(m_lineTypes.size() == m_lines.size());
  temperatureDataInfo();
  assert(m_lineTypes.size() == m_lines.size());
  strainDataInfo();
  assert(m_lineTypes.size() == m_lines.size());
  attachedMaterialsInfo();
  assert(m_lineTypes.size() == m_lines.size());
  boundaryInfo();
  assert(m_lineTypes.size() == m_lines.size());
  upscalingInfo();
  assert(m_lineTypes.size() == m_lines.size());
  zoomInInfo();
  assert(m_lineTypes.size() == m_lines.size());
  FaultsInfo();
  assert(m_lineTypes.size() == m_lines.size());
  faultPressureInfo();
  assert(m_lineTypes.size() == m_lines.size());
}

void CDumpModel::addLine(TLineType lineType, const QString& line)
{
  m_lineTypes.push_back(lineType);
  m_lines.push_back(line);
}

std::vector <CDumpModel::TLineType> CDumpModel::getLineTypes() const
{
  return m_lineTypes;
}

std::vector <QString> CDumpModel::getLines() const
{
  return m_lines;
}

// private

void CDumpModel::generalModelInfo(const QString& modelName,
  const QString& fileLocation)
{
  addLine(boldUnderlinedText, "General model information");
  addLine(newLine, "");
  addLine(boldText, "Model Name: ");
  addLine(text, modelName);
  addLine(newLine, "");
  addLine(boldText, "File Location: ");
  addLine(text, fileLocation);
  addLine(newLine, "");
  addLine(text, m_model.ProjectInfo());
  addLine(newLine, "");
  addLine(boldText, "ModelType: ");

  const CTetraModel* tetraModel = dynamic_cast <const CTetraModel*> (&m_model);

  addLine(text, tetraModel ? "Tetrahedron" : "Hexahedron");
  addLine(newLine, "");
  addLine(boldText, "Number of formations: ");
  addLine(text, QString("%1").arg(((TFormationBaseEntry*)(m_model.GraphEntry(MD_BASE_FORMATION)))->EntryNodes().size()));
  addLine(newLine, "");
  addLine(boldText, "Number of faults: ");
  int nFaults = 0;
  THorizonBaseEntry* pEntry = (THorizonBaseEntry*)m_model.GraphEntry(MD_BASE_HORIZON);
  for(THorizonBaseEntry::TNodeSet::const_iterator it = pEntry->EntryNodes().begin(); it != pEntry->EntryNodes().end(); ++it)
  {
  if((*it)->Slip())
      ++nFaults;
  }
  addLine(text, QString("%1").arg(nFaults));
  addLine(newLine, "");

  if (m_model.IsMesh())
  {
  const CMeshBase& meshBase = m_model.Mesh();
  QString minMax;
  CLengthQuantity lq;
  CUnitNode unit;

  addLine(boldText, "Easting: ");
  minMax = QString("%1 <-> %2 %3").
      arg(lq.Convert(meshBase.Min().Y(), unit.Unit(), CDoubleQuantity::SI_UNIT), 0, 'G').
      arg(lq.Convert(meshBase.Max().Y(), unit.Unit(), CDoubleQuantity::SI_UNIT), 0, 'G').
      arg(lq.UnitName(unit.Unit()).c_str());
  addLine(text, minMax);
  addLine(newLine, "");
  addLine(boldText, "Northing: ");
  minMax = QString("%1 <-> %2 %3").
      arg(lq.Convert(meshBase.Min().X(), unit.Unit(), CDoubleQuantity::SI_UNIT), 0, 'G').
      arg(lq.Convert(meshBase.Max().X(), unit.Unit(), CDoubleQuantity::SI_UNIT), 0, 'G').
      arg(lq.UnitName(unit.Unit()).c_str());
  addLine(text, minMax);
  addLine(newLine, "");
  addLine(boldText, "Depth: ");
  minMax = QString("%1 <-> %2 %3").
      arg(lq.Convert(meshBase.Min().Z(), unit.Unit(), CDoubleQuantity::SI_UNIT), 0, 'G').
      arg(lq.Convert(meshBase.Max().Z(), unit.Unit(), CDoubleQuantity::SI_UNIT), 0, 'G').
      arg(lq.UnitName(unit.Unit()).c_str());
  addLine(text, minMax);
  addLine(newLine, "");
  addLine(text, QString("%1 nodes").arg(meshBase.Mesh().NodeSize()));
  addLine(newLine, "");
  addLine(text, QString("%1 elements").arg(meshBase.Mesh().ElementSize()));
  addLine(newLine, "");
  addLine(boldText, "Element volumes:");
  addLine(newLine, "");
  double dSum = 0;
  double dMax = -DBL_MAX;
  double dMin = DBL_MAX;
  for(int i = 0; i < meshBase.Mesh().ElementSize(); ++i)
  {
      double dVol = meshBase.Mesh().Element(i).Size();
      dSum += dVol;
      dMax = dVol > dMax ? dVol : dMax;
      dMin = dVol < dMin ? dVol : dMin;
  }
  addLine(text, QString("Max %1").arg(dMax));
  addLine(newLine, "");
  addLine(text, QString("Min %1").arg(dMin));
  addLine(newLine, "");
  addLine(text, QString("Average %1").arg(dSum / meshBase.Mesh().ElementSize()));
  addLine(newLine, "");
  }
  else 
  {
  addLine(text, "\t\tNo Mesh");
  addLine(newLine, "");
  }

  addLine(newLine, "");
}

void CDumpModel::nonLinCalcSettingsInfo()
{
  addLine(boldUnderlinedText, "NonLinear Calculation Settings");
  addLine(newLine, "");
  addLine(boldText, "Automatic steps\tMax relative step size\tSteps\tScale Factor\tConverg. Norm Value\tMaxIters\tIter. "
  "Scheme\tConverg. Norm Type");
  addLine(newLine, "");

  QString temp;
  const CDepletionStage* pStage = 0;

  for (size_t i = 0; i < m_vcStages.size(); ++i)
  {
  pStage = m_vcStages[i]; 
  temp = QString("%1:").arg(pStage->Name());
  addLine(boldText, temp);
  addLine(newLine, "");
  addLine(text, QString("%1").arg(pStage->AutomaticStepSizes() ? "Yes" : "No"));
  addLine(text, QString("\t\t%1").arg(pStage->MaxAutomaticStepSize(), 0, 'G'));
  temp = QString("\t\t\t%1").arg(pStage->NLSteps());
  addLine(text, temp);
  temp = QString("\t%1").arg(pStage->NLStepScalingFactor(), 0, 'G');
  addLine(text, temp);
  temp = QString("\t\t%1").arg(pStage->NLConvNormVal(), 0, 'G');
  addLine(text, temp);
  temp = QString("\t\t\t%1").arg(pStage->NLIterationSteps());
  addLine(text, temp);
  temp = QString("\t\t%1").
      arg(pStage->IterationSchemeTag(pStage->NLIterationScheme()).c_str());
  addLine(text, temp);
  temp = QString("\t\t%1").
      arg(pStage->ConvergenceTypeTag(pStage->NLConvergenceType()).c_str()); 
  addLine(text, temp);
  addLine(newLine, "");
  }

  addLine(newLine, "");
  addLine(boldText, "Use only 1 time step per depletion stage for nonlin, if all rock is linear elastic: ");
  addLine(text, m_model.AllowLinearCalc() ? "Yes" : "No");
  addLine(newLine, "");
  addLine(boldText, "Large deformations: ");
  addLine(text, m_model.LargeDeformations() ? "Yes" : "No");

  addLine(newLine, "");
  addLine(newLine, "");
}

void CDumpModel::analysisOptionsInfo()
{
  QString temp;

  addLine(boldUnderlinedText, "Analysis Options");
  addLine(newLine, "");
  addLine(text, "Element Order: ");

  switch (m_model.ElementOrder())
  {
  case CElementOrder::EO_LINEAR:
      addLine(text, "Linear");
      break;
  case CElementOrder::EO_QUADRATIC:
      addLine(text, "Quadratic");
      break;
  case CElementOrder::EO_INIQUAD:
      addLine(text, "Initialize with Quadratic, then Linear");
      break;
  default:
      assert(false);
  }

  addLine(newLine, "");
  addLine(text, "Salt, stress initially relaxed: ");

  if (m_model.CanDisableSaltInitRelax())
  {
  m_model.SaltInitRelax() ? addLine(text, "Yes") : addLine(text, "No");
  }
  else
  {
  addLine(text, "Always");
  }

  addLine(newLine, "");
  addLine(text, "Enhanced assumed strain method: ");
  m_model.NoCSHE() ? addLine(text, "No") : addLine(text, "Yes");
  addLine(newLine, "");
  addLine(text, "Parallel solver: ");

  if (m_model.UseParallelSolver())
  {
  temp = QString("Yes %1 threads").arg(m_model.MaxNumSolverThreads());
  addLine(text, temp);
  }
  else
  {
  addLine(text, "No");
  }

  addLine(newLine, "");
  addLine(text, "Separate linear compaction and decompaction parameters: ");
  m_model.UseDecompactionParameters() ? addLine(text, "Yes") : addLine(text, "No");

  addLine(newLine, "");
  addLine(text, QString("Generate output stream file: %1").arg(m_model.WriteOutputStreamFile() ? "Yes" : "No"));

  addLine(newLine, "");
  addLine(text, QString("Enable result cache: %1").arg(m_model.getEnableResultCache() ? "Yes" : "No"));
  if(m_model.getEnableResultCache())
  addLine(text, QString(", cache size: %1 MB").arg(m_model.getResultCacheSize()));

  addLine(newLine, "");
  addLine(newLine, "");
}

void CDumpModel::globalDataInfo()
{
  CLengthQuantity lq;
  CSinglePressure sp;
  CStressGradientQuantity sgq;
  CTemperatureQuantity tq;
  CTemperatureGradientQuantity tgq;

  QString temp;
  const CGlobalPressure &gp = m_model.GlobalPressure();
  const CGlobalTemperature &gt = m_model.InitialTemperature();
  const CGlobalInitialStressNode &gis = m_model.GlobalInitialStress();
  CUnitNode unit;

  addLine(boldUnderlinedText, "Global Data");
  addLine(newLine, "");
  addLine(text, "Global pore pressure: ");
  temp = QString("%1 %2 @ %3 %4; %5 %6").
  arg(gp.Constant().ReferenceValue().Value(unit.Unit()), 0, 'G').
  arg(sp.UnitName(unit.Unit()).c_str()).
  arg(gp.Constant().ReferenceDepth().Value(unit.Unit()), 0, 'G').
  arg(lq.UnitName(unit.Unit()).c_str()).
  arg(gp.Constant().Gradient().Value(unit.Unit()), 0, 'G').
  arg(sgq.UnitName(unit.Unit()).c_str());
  addLine(text, temp);
  addLine(newLine, "");
  addLine(text, "Global temperature: ");
  temp = QString("%1 %2 @ %3 %4; %5 %6").
  arg(gt.Constant().ReferenceValue().Value(unit.Unit()), 0, 'G').
  arg(tq.UnitName(unit.Unit()).c_str()).
  arg(gt.Constant().ReferenceDepth().Value(unit.Unit()), 0, 'G').
  arg(lq.UnitName(unit.Unit()).c_str()).
  arg(gt.Constant().Gradient().Value(unit.Unit()), 0, 'G').
  arg(tgq.UnitName(unit.Unit()).c_str());
  addLine(text, temp);
  addLine(newLine, "");
  addLine(text, "Weight of seawater: ");
  temp = QString("%1 %2 %3 %4").
  arg(gis.WaterSurfaceDepth().Value(unit.Unit()), 0, 'G').
  arg(lq.UnitName(unit.Unit()).c_str()).
  arg(gis.WaterColumnStressGradient().Value(unit.Unit()), 0, 'G').
  arg(sgq.UnitName(unit.Unit()).c_str());
  addLine(text, temp);
  addLine(newLine, "");
  addLine(newLine, "");
}

void CDumpModel::depletionStageInfo()
{
  QString temp;

  addLine(boldUnderlinedText, "Depletion Stages");
  addLine(newLine, "");

  const CDepletionStage* pStage = 0;

  for (size_t i = 0; i < m_vcStages.size(); ++i)
  {
  pStage = m_vcStages[i];

  const CGeomecTime &time = pStage->Time();

  addLine(text, pStage->Name());
  temp = QString("\t%1 %2\t").arg(time.GetYear()).arg(time.GetMonth());
  addLine(text, temp);

  switch (pStage->OutputType())
  {
      case CDepletionStage::RESULTS:
    addLine(text, "Results");
    break;
      case CDepletionStage::PHASE:
    addLine(text, "Phase");
    break;
      case CDepletionStage::BRANCH:
    addLine(text, "Branch");
    break;
  }

  addLine(newLine, "");
  }

  addLine(newLine, "");
}

void CDumpModel::pressureDataInfo()
{
  QString temp;
  CLengthQuantity lq;
  CSinglePressure sp;
  CStressGradientQuantity sgq;
  CUnitNode unit;

  addLine(boldUnderlinedText, "Pressure Data Over Depletion Stages");
  addLine(newLine, "");

  for (size_t i = 0; i < m_vcFormations.size(); ++i)
  {
  addLine(boldText, m_vcFormations[i]->Name());
  addLine(newLine, "");
  
  for (size_t j = 0; j < m_vcStages.size(); ++j)
  {
      const CPressure* pPressure =
    &m_vcFormations[i]->Pressure(*m_vcStages[j]);

      temp = QString("%1: ").arg(m_vcStages[j]->Name());
      addLine(text, temp);
      if(pPressure->DistributedSize() > 1)
    addLine(text, "Pointsets: ");
      else
    addLine(text, "Pointset: ");

      if (pPressure->DistributedSize() > 0)
      {
    std::set<const IPointSet*> stPointsets;
    for(size_t k = 0; k < pPressure->DistributedSize(); ++k)
          stPointsets.insert(&pPressure->DistributedValue(k).PointSet());

    for(std::set<const IPointSet*>::const_iterator it = stPointsets.begin(); it != stPointsets.end(); ++it)
    {
          if(stPointsets.size() > 1)
      addLine(newLine, "");
          addLine(text, QString("\"%1\"").arg((*it)->Name()));
          if (pPressure->DistributedOnly())
      addLine(text, ", Extrapolation");
    }
      }
      else
      {
    addLine(text, "No; ");
      }

      if (pPressure->DistributedSize() == 0 ||
    (pPressure->DistributedSize() > 0 && !pPressure->DistributedOnly()))
      {
    switch (pPressure->Type())
    {
          case CPressure::CONSTANT_MODE:
      temp = QString("Gradient - %1 %2 @ %3 %4; %5 %6").
              arg(pPressure->Constant().ReferenceValue().Value(unit.Unit()), 0, 'G').
              arg(sp.UnitName(unit.Unit()).c_str()).
              arg(pPressure->Constant().ReferenceDepth().Value(unit.Unit()), 0, 'G').
              arg(lq.UnitName(unit.Unit()).c_str()).
              arg(pPressure->Constant().Gradient().Value(unit.Unit()), 0, 'G').
              arg(sgq.UnitName(unit.Unit()).c_str());
      addLine(text, temp);
      break;
          case CPressure::REPEAT_MODE:
      if (m_vcStages[j]->IsMarkedAsInitial())
      {
              addLine(text, "Global");
      }
      else
      {
              addLine(text, "Same as previous");
      }
      break;
          case CPressure::GWC_MODE:
      temp = QString("GWC - %1 %2 @ %3 %4; %5 %6 (upper), %7 %8 (lower); ").
              arg(pPressure->GWC().ContactPressure().Value(unit.Unit()), 2, 'f').
              arg(sp.UnitName(unit.Unit()).c_str()).
              arg(pPressure->GWC().ContactDepth().Value(unit.Unit()), 0, 'G').
              arg(lq.UnitName(unit.Unit()).c_str()).
              arg(pPressure->GWC().UpperGradient().Value(unit.Unit()), 0, 'G').
              arg(sgq.UnitName(unit.Unit()).c_str()).
              arg(pPressure->GWC().LowerGradient().Value(unit.Unit()), 0, 'G').
              arg(sgq.UnitName(unit.Unit()).c_str());
      addLine(text, temp);
      break;
          default:
      assert(false);
      break;
    }
      }

      addLine(newLine, "");
  }
  }

  addLine(newLine, "");
}

// waij TFS 96130
//
void CDumpModel::faultPressureInfo()
{
  CGlobalUnitNode globalUnitNode;
  addLine(boldUnderlinedText, "Faults Pressure Data");
  addLine(newLine, "");

  for (size_t i = 0; i < m_vcFaults.size(); ++i)
  {
  const CHorizonBase* pFault= m_vcFaults[i];

  addLine(boldText, pFault->Name());
  addLine(text, ": ");
  addLine(newLine,"");

  if ( ! pFault->Slip() )
  {
      addLine(text, "no slip");
      continue;
  }

  double dPrevRefDepth= std::numeric_limits<double>::quiet_NaN();
  double dPrevRefPressure= std::numeric_limits<double>::quiet_NaN();
  double dPrevGradient= std::numeric_limits<double>::quiet_NaN();
  enum {e_unknown=-1,e_distr=1,e_max, e_min, e_maxgrad, e_grad}
      ePrevState=e_unknown;

  for (size_t j = 0; j < m_vcStages.size(); ++j)
  {
      QString temp;

      const CDepletionStage *pStage = m_vcStages[j];

      temp = QString("%1: ").arg(pStage->Name());
      addLine(text, temp);

      const CFaultPressure& faultPressure= pFault->Pressure( *pStage);

      if (faultPressure.DistributedSize() > 0)
          addLine(text, " pointset: ");

      if ( faultPressure.DistributedOnly() )
      {
    if (ePrevState != e_distr)
    {
          addLine(text, "extrapolated");
          ePrevState= e_distr;
    }
    else
          addLine(text, "same as previous");
      }
      else
      {
    switch ( faultPressure.Mode() )
    {
    case CFaultPressure::MT_MAXIMUM:
          if (ePrevState != e_max)
          {
      addLine(text, "default: ");
      addLine(text, "maximum either side");
      ePrevState= e_max;
          }
          else
      addLine(text, "same as previous");
          break;
    case CFaultPressure::MT_MINIMUM:
          if ( ePrevState != e_min )
          {
      addLine(text, "default: ");
      addLine(text, "minimum either side");
      ePrevState= e_min;
          }
          else
      addLine(text, "same as previous");
          break;
    case CFaultPressure::MT_MAXGRADIENT:
          if (ePrevState != e_maxgrad)
          {
      addLine(text, "default: ");
      addLine(text, "maximum in fault");
      ePrevState= e_maxgrad;
          }
          else
      addLine(text, "same as previous");
          break;
    case CFaultPressure::MT_GRADIENT:
          double dRefDepth = faultPressure.Gradient().ReferenceDepth().Value(globalUnitNode.unitNode().Unit());
          double dRefPressure = faultPressure.Gradient().ReferencePressure().Value(globalUnitNode.unitNode().Unit());
          double dGradient = faultPressure.Gradient().Gradient().Value(globalUnitNode.unitNode().Unit());
          QString strDepthUnit =
      faultPressure.Gradient().ReferenceDepth().UnitName(globalUnitNode.unitNode().Unit()).c_str();
          QString strPressureUnit =
      faultPressure.Gradient().ReferencePressure().UnitName(globalUnitNode.unitNode().Unit()).c_str();
          QString strGradientUnit =
      faultPressure.Gradient().Gradient().UnitName(globalUnitNode.unitNode().Unit()).c_str();

          if 
          (  ePrevState != e_grad 
          || dRefDepth != dPrevRefDepth
          || dRefPressure != dPrevRefPressure
          || dGradient != dPrevGradient
          )
          {
      addLine(text, "default: ");
      addLine(text, "user gradient");
      temp= QString(", reference depth=%1 ").arg(dRefDepth);
      addLine(text, temp);
      addLine(text, strDepthUnit);
      temp= QString(", reference pressure=%1 ").arg(dRefPressure);
      addLine(text, temp);
      addLine(text, strPressureUnit);
      temp= QString(", gradient=%1 ").arg(dGradient);
      addLine(text, temp);
      addLine(text, strGradientUnit);

      ePrevState= e_grad;
      dPrevRefDepth= dRefDepth;
      dPrevRefPressure= dRefPressure;
      dPrevGradient= dGradient;
          }
          else
      addLine(text, "same as previous");
          break;

    } // switch

    assert(ePrevState!= e_unknown);
      } // else
      addLine(newLine, "");
  } // for stages        
  } // for faults

  addLine(newLine, "");
}

void CDumpModel::temperatureDataInfo()
{
  QString temp;
  CLengthQuantity lq;
  CTemperatureQuantity tq;
  CTemperatureGradientQuantity tgq;
  CUnitNode unit;

  addLine(boldUnderlinedText, "Temperature Data Over Depletion Stages");
  addLine(newLine, "");

  for (size_t i = 0; i < m_vcFormations.size(); ++i)
  {
  addLine(boldText, m_vcFormations[i]->Name());
  addLine(newLine, "");
  
  for (size_t j = 0; j < m_vcStages.size(); ++j)
  {
      const CTemperature* pTemperature =
    &m_vcFormations[i]->UserTemperature(*m_vcStages[j]);

      temp = QString("%1: ").arg(m_vcStages[j]->Name());
      addLine(text, temp);
      if(pTemperature->DistributedSize() > 1)
    addLine(text, "Pointsets: ");
      else
    addLine(text, "Pointset: ");

      if (pTemperature->DistributedSize() > 0)
      {
    std::set<const IPointSet*> stPointsets;
    for(size_t k = 0; k < pTemperature->DistributedSize(); ++k)
          stPointsets.insert(&pTemperature->DistributedValue(k).PointSet());

    for(std::set<const IPointSet*>::const_iterator it = stPointsets.begin(); it != stPointsets.end(); ++it)
    {
          if(stPointsets.size() > 1)
      addLine(newLine, "");
          addLine(text, QString("\"%1\"").arg((*it)->Name()));
          if (pTemperature->DistributedOnly())
      addLine(text, ", Extrapolation");
    }
      }
      else
      {
    addLine(text, "No; ");
      }

      if (pTemperature->DistributedSize() == 0 ||
    (pTemperature->DistributedSize() > 0 && !pTemperature->DistributedOnly()))
      {
    switch (pTemperature->Type())
    {
          case CPressure::CONSTANT_MODE:
      temp = QString("Gradient - %1 %2 @ %3 %4; %5 %6").
              arg(pTemperature->Constant().ReferenceValue().Value(unit.Unit()), 0, 'G').
              arg(tq.UnitName(unit.Unit()).c_str()).
              arg(pTemperature->Constant().ReferenceDepth().Value(unit.Unit()), 0, 'G').
              arg(lq.UnitName(unit.Unit()).c_str()).
              arg(pTemperature->Constant().Gradient().Value(unit.Unit()), 0, 'G').
              arg(tgq.UnitName(unit.Unit()).c_str());
      addLine(text, temp);
      break;
          case CPressure::REPEAT_MODE:
      if (m_vcStages[j]->IsMarkedAsInitial())
      {
              addLine(text, "Global");
      }
      else
      {
              addLine(text, "Same as previous");
      }
      break;
          case CPressure::GWC_MODE:
          default:
      assert(false);
      break;
    }
      }

      addLine(newLine, "");
  }
  }

  addLine(newLine, "");
}

void CDumpModel::strainDataInfo()
{
  QString temp;

  addLine(boldUnderlinedText, "Strain Loading Over Depletion Stages");
  addLine(newLine, "");
  
  for (size_t i = 0; i < m_vcFormations.size(); ++i)
  {
  addLine(boldText, m_vcFormations[i]->Name());
  addLine(newLine, "");

  for (size_t j = 0; j < m_vcStages.size(); ++j)
  {
      // skip initial stage

      if (j == 0)
      {
    continue;
      }

      const CStrainLoad* pStrainLoad =
    &m_vcFormations[i]->Strain(*m_vcStages[j]);

      temp = QString("%1: ").arg(m_vcStages[j]->Name());
      addLine(text, temp);
      if(pStrainLoad->DistributedSize() > 1)
    addLine(text, "Pointsets: ");
      else
    addLine(text, "Pointset: ");

      if (pStrainLoad->DistributedSize() > 0)
      {
    std::set<const IPointSet*> stPointsets;
    for(size_t k = 0; k < pStrainLoad->DistributedSize(); ++k)
    {
          const CValueType* pVT = dynamic_cast<const CValueType*>(&pStrainLoad->DistributedValue(k));
          if(pVT)
      stPointsets.insert(&pVT->PointSet());
    }

    for(std::set<const IPointSet*>::const_iterator it = stPointsets.begin(); it != stPointsets.end(); ++it)
    {
          if(stPointsets.size() > 1)
      addLine(newLine, "");
          addLine(text, QString("\"%1\"").arg((*it)->Name()));
          if (pStrainLoad->DistributedOnly())
      addLine(text, ", Extrapolation");
    }
      }
      else
      {
    if (j == 1)
    {
          addLine(text, "No");
    }
    else
    {
          addLine(text, "No; Same as previous");
    }
      }

      addLine(newLine, "");
  }
  }
  addLine(newLine, "");
}

void CDumpModel::attachedMaterialsInfo()
{
  QString temp;
  const CMaterialServer* pMatServer = 0;

  addLine(boldUnderlinedText, "Material Data");
  addLine(newLine, "");

  for (size_t i = 0; i < m_vcFormations.size(); ++i)
  {
  const CFormationBase *pFormation = m_vcFormations[i];

  addLine(boldText, pFormation->Name());
  addLine(newLine, "");

  for (size_t j = 0; j < m_vcStages.size(); ++j)
  {
      const CDepletionStage *pStage = m_vcStages[j];

      temp = QString("%1: ").arg(pStage->Name());
      addLine(boldText, temp);

      if (&(pFormation->Material(*pStage)) != pMatServer)
      {
    pMatServer = &(pFormation->Material(*pStage));

    materialInfo(pMatServer);
      }
      else
      {
    addLine(text, "same as previous");
      }

      addLine(newLine, "");
      addLine(newLine, "");
  }
  }
}

// waij TFS 96130
//
void CDumpModel::materialInfo(const CMaterialServer* pMatServer)
{
  // "cam-clay", "linear", etc.
  if(pMatServer->LibraryMaterial())
  {
  QString temp = QString("%1 (%2): ").arg(pMatServer->MaterialName()).arg(
      MaterialModelName( (mlMatModel )(pMatServer->MaterialModel())) );
  addLine(text,temp);
  CGlobalUnitNode globalUnitNode;
  const CMaterialServer::TValueTypePairVec& vcValueTypePairs = pMatServer->ValueTypePairVec();
  for(size_t i = 0; i < pMatServer->LibraryMaterial()->ParameterSize(); ++i)
  {
      addLine(newLine, "");
      const CLibraryMaterialParameter& param = pMatServer->LibraryMaterial()->LibraryMaterial().Parameter(i);
      int nUnitDef;
      switch(globalUnitNode.unitNode().Unit())
      {
      case IQuantityDouble::SI_UNIT:
    nUnitDef = MLUD_SI;
    break;
      case IQuantityDouble::FIELD_UNIT:
    nUnitDef = MLUD_FIELD;
    break;
      default:
    assert(false);
      }
      temp = QString("   %1 %2 %3").arg(param.Name()).arg(param.ValueToUserUnit(nUnitDef)).arg(param.UnitName(nUnitDef));
      addLine(text, temp);

      for(int j = 0; j < vcValueTypePairs.size(); ++j)
      {
    if(vcValueTypePairs[j].second == param.ValueTypeID())
    {
          const CValueType& vt = *vcValueTypePairs[j].first;
          IProgressBase progress1, progress2;
          IValueDomainScalar::TMinMax mm = vt.Component().ScalarData().MinMax(progress1, globalUnitNode.unitNode().Unit());
          temp = QString(", pointset \"%1\", min = %2 %3, max = %4 %5, average = %6 %7").arg(vt.PointSet().Name()).
                                    arg(mm.first.Value()).
                                    arg(vt.Component().UnitName(globalUnitNode.unitNode().Unit())).
                                    arg(mm.second.Value()).
                                    arg(vt.Component().UnitName(globalUnitNode.unitNode().Unit())).
                                    arg(vt.Component().ScalarData().Average(progress2, globalUnitNode.unitNode().Unit()).Value()).
                                    arg(vt.Component().UnitName(globalUnitNode.unitNode().Unit()));
          addLine(text, temp);
          break;
    }
      }
  }
  }
  else
  {
  addLine(text,"no material");
  }
}

void CDumpModel::boundaryInfo()
{
  const CBoundaryBase& boundary = m_model.Boundary();

  addLine(boldUnderlinedText, "Boundary");
  addLine(newLine, "");

  addLine(boldText, "State: ");
  switch(boundary.State())
  {
  case CBoundaryBase::DEFAULT_DEFINED:
  addLine(text, "default");
  break;
  case CBoundaryBase::BEST_FIT:
  addLine(text, "best fit");
  break;
  case CBoundaryBase::USER_DEFINED:
  addLine(text, "user-defined");
  break;
  case CBoundaryBase::ROTATED:
  addLine(text, "rotated");
  break;
  default:
  assert(false);
  }

  addLine(newLine, "");
  addLine(newLine, "");

  CGlobalUnitNode globalUnitNode;

  if(globalUnitNode.unitNode().Unit() == IQuantityDouble::SI_UNIT)
  {
  addLine(text, QString("Length: %1 m").arg(boundary.Length()));
  addLine(newLine, "");
  addLine(text, QString("Width: %1 m").arg(boundary.Width()));
  addLine(newLine, "");
  }
  else
  {
  assert(globalUnitNode.unitNode().Unit() == IQuantityDouble::FIELD_UNIT);
  addLine(text, QString("Length: %1 ft").arg(boundary.Length() * FF_FACTOR_LENGTH));
  addLine(newLine, "");
  addLine(text, QString("Width: %1 ft").arg(boundary.Width() * FF_FACTOR_LENGTH));
  addLine(newLine, "");
  }

  addLine(text, QString("Azimuth: %1 degrees").arg(boundary.Azimuth()));
  addLine(newLine, "");

  const CInterfaceBoundary* pBoundary = dynamic_cast<const CInterfaceBoundary*>(&boundary);
  if(pBoundary)
  {
  addLine(newLine, "");
  addLine(text, QString("Boundary has interfaces: %1").arg(pBoundary->HasInterfaces() ? "yes" : "no"));
  addLine(newLine, "");

  const CPressureSupportNode& psn = pBoundary->PressureSupportNode();
  CPressureSupportNode::TSurfaceVec vcSurfaces = psn.getSurfaces();

  addLine(newLine, "");
  addLine(boldText, "Supports");
  addLine(newLine, "");
  addLine(boldText, "Surface location\t\tSupport");
  addLine(newLine, "");
  for(int i = 0; i < vcSurfaces.size(); ++i)
  {
      const CPressureSupportNode::CFaceSupportDef& fsd = psn.FaceSupportDef(CPressureSupportNode::CSurfaceDef(*vcSurfaces[i].first));

      QString strSupportType;
      switch(fsd.SupportType())
      {
      case CPressureSupportNode::CFaceSupportDef::ST_NORMAL:
    strSupportType = "Normally supported";
    break;
      case CPressureSupportNode::CFaceSupportDef::ST_FULL:
    strSupportType = "Fully supported";
    break;
      case CPressureSupportNode::CFaceSupportDef::ST_HORIZONTAL:
    strSupportType = "Supported in Northing and Easting";
    break;
      case CPressureSupportNode::CFaceSupportDef::ST_VERTICAL:
    strSupportType = "Vertically supported";
    break;
      default:
    strSupportType = "Not supported";
      }

      QString strLocation;
      switch(fsd.SurfaceLocation())
      {
      case CPressureSupportNode::SL_SIDE:
    strLocation = "Side";
    break;
      case CPressureSupportNode::SL_TOP:
    strLocation = "Top";
    break;
      case CPressureSupportNode::SL_BOTTOM:
    strLocation = "Bottom";
    break;
      default:
    assert(false);
      }

      addLine(text, QString("%1\t\t\t%2").arg(strLocation).arg(strSupportType));
      addLine(newLine, "");
  }

  addLine(newLine, "");

  addLine(newLine, "");
  addLine(boldText, "Prescribed displacements");
  addLine(newLine, "");
  addLine(boldText, "Stage\tX\tY\tZ\tdX\tdY\tdZ");
  addLine(newLine, "");
  const CDisplacementSupportNode& dsn = pBoundary->DisplacementSupportNode();
  CDisplacementSupportNode::TCornerNodeVec vcCorners = dsn.getCornerNodes();
  const geo::IPoint& ptRef = dsn.ReferencePoint();

  for(int i = 0; i < vcCorners.size(); ++i)
  {
      const geo::IPoint& pt = *vcCorners[i];
      const CDisplacementSupportNode::CNodalSupportDef* pNsd = dsn.NodalSupportDef(pt);
      for(CDepletionStageEntry::const_iterator it = m_model.DepletionStageEntry().begin(); it != m_model.DepletionStageEntry().end(); ++it)
      {
    const CDepletionStage& ds = *it;
    if(!ds.Initial())
    {
          geo::CVector vec = dsn.DisplacementFromDistribution(pt, ds);
          if(vec.Empty() && pNsd)
          {
      geo::CMatrix matPremult = dsn.CreateGlobalTensorPreMultMatrix(ds);
      const CDisplacementSupportNode::CNodalSupportDef::CSupportDef& sd = pNsd->SupportDef(ds);
      vec = sd.Vector(pt, matPremult, ptRef);
          }

          if(!vec.Empty())
          {
      addLine(text, QString("%1\t%2\t%3\t%4\t%5\t%6\t%7").arg(ds.Index()).arg(pt.X()).arg(pt.Y()).arg(pt.Z()).arg(vec.X()).arg(vec.Y()).arg(vec.Z()));
      addLine(newLine, "");
          }
    }
      }
  }

  addLine(newLine, "");
  }
}

void CDumpModel::upscalingInfo()
{
  addLine(boldUnderlinedText, "Upscaling");
  addLine(newLine, "");
  addLine(newLine, "");

  const CUpscalingEntry& entry = (const CUpscalingEntry&)(*m_model.GraphEntry(MD_BASE_UPSCALING));
  const CUpscalingEntry::TNodeSet& stNodes = entry.EntryNodes();
  for(CUpscalingEntry::TNodeSet::const_iterator it = stNodes.begin(); it != stNodes.end(); ++it)
  {
  const IUpscalingNodeBase& unb = **it;
  addLine(boldText, unb.Name());
  addLine(newLine, "");
  addLine(text, "Type: ");
  switch(unb.FactoryType())
  {
  case FACTORYTYPE_THINLAYERUPSCALING:
      addLine(text, "Thin layer upscaling");
      break;
  case FACTORYTYPE_CAMCLAYUPSCALING:
      addLine(text, "Cam clay upscaling");
      break;
  default:
      assert(false);
      addLine(text, "(unknown)");
  }
  addLine(newLine, "");
  std::set<const CFormationBase*> stFormations = unb.FormationsBranch().Links<const CFormationBase>();
  if(!stFormations.empty())
  {
      addLine(text, "Formations:");
      addLine(newLine, "");
      for(std::set<const CFormationBase*>::const_iterator it2 = stFormations.begin(); it2 != stFormations.end(); ++it2)
      {
    addLine(text, (*it2)->Name());
    addLine(newLine, "");
      }
      addLine(newLine, "");
  }

  std::set<const CPointSet*> stPointsets = unb.PointsetRangesBranch().Links<const CPointSet>();
  if(!stPointsets.empty())
  {
      addLine(text, "Pointsets:");
      addLine(newLine, "");
      for(std::set<const CPointSet*>::const_iterator it2 = stPointsets.begin(); it2 != stPointsets.end(); ++it2)
      {
    addLine(text, (*it2)->Name());
    addLine(newLine, "");
      }
      addLine(newLine, "");
  }
  }
}

void CDumpModel::zoomInInfo()
{
  const CZoominModelEntry* pEntry = (const CZoominModelEntry*)m_model.GraphEntry(MD_BASE_ZOOMIN_MODEL);
  if(pEntry)
  {
  addLine(boldUnderlinedText, "Zoomin models");
  addLine(newLine, "");

  const CZoominModelEntry::TNodeSet& stNodes = pEntry->EntryNodes();
  for(CZoominModelEntry::TNodeSet::const_iterator it = stNodes.begin(); it != stNodes.end(); ++it)
  {
      addLine(newLine, "");
      const CZoominModelPlaceHolder& ph = **it;
      const CModelBase& childModel = ph.ChildModel();
      CDumpModel dm(childModel);
      dm.dump(ph.Name(), "<zoomin model>");
      std::vector<TLineType> vcLineTypes = dm.getLineTypes();
      std::vector<QString> vcLines = dm.getLines();
      assert(vcLineTypes.size() == vcLines.size());
      for(int i = 0; i < vcLines.size(); ++i)
      {
    addLine(text, "  ");
    addLine(vcLineTypes[i], vcLines[i]);
      }
  }

  addLine(newLine, "");
  }
}

// waij TFS 96130
//
void CDumpModel::FaultsInfo()
{
  addLine(boldUnderlinedText, "Faults Data");
  addLine(newLine, "");

  for (size_t i = 0; i < m_vcFaults.size(); ++i)
  {
  const CTetraSubHorizon* pFault= m_vcFaults[i];

  addLine(boldText, pFault->Name());
  addLine(text, ": ");
  switch ( pFault->SlipType() )
  {
      case CHorizonBase::SLIP:
    addLine(text, "Complete slip");
    break;
      case CHorizonBase::STICK:
    addLine(text, "Complete stick");
    break;
      case CHorizonBase::USER:
    addLine(text, "Mohr-Coulomb");
    break;
      case CHorizonBase::FRACTURE:
    addLine(text, "Fracture");
    break;
      default:
    assert(false);
  }

  QString temp;
  const CDepletionStage& initialDepletionStage =
      m_model.InitialDepletionStage();

  if (pFault->Slip() &&
      pFault->hasFaultParameters(initialDepletionStage))
  {
      CGlobalUnitNode globalUnitNode;

      const CFaultParametersNode& faultParametersNode =
    pFault->FaultParameters(initialDepletionStage);

      if ( faultParametersNode.DistributedCohesion() )
      {
    addLine(text, ", cohesion: pointset ");
      }
      else
      {
    QString cohesionUnit = faultParametersNode.Cohesion().
          UnitName(globalUnitNode.unitNode().Unit()).c_str();

    double cohesion =
          faultParametersNode.Cohesion().Value(globalUnitNode.unitNode().Unit());
    temp= QString(", cohesion=%1 ").arg(cohesion);
    addLine(text, temp);
    addLine(text, cohesionUnit);
      }

      if ( faultParametersNode.DistributedFriction() )
      {
    addLine(text, ", friction: pointset ");
      }
      else
      {
    double frictionAngle =
          faultParametersNode.FrictionAngle().Value(globalUnitNode.unitNode().Unit());
    QString frictionAngleUnit = faultParametersNode.FrictionAngle().
          UnitName(globalUnitNode.unitNode().Unit()).c_str();

    temp= QString(", friction angle=%1 ").arg(frictionAngle);
    addLine(text, temp);
    addLine(text, frictionAngleUnit);
      }
  }

  addLine(newLine, "");
  }

  addLine(newLine, "");
}
