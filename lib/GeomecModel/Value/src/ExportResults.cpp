#include "ExportResults.h"
#include "ExportResultData.h"
#include "LocalResult.h"
#include "Result.h"
#include "ExportSelection.h"
#include "ResultExporter.h"
#include "FormationPlane.h"
#include "WellCasingRTCI.h"
#include "NonMeshedSurface.h"
#include "3DHorizon.h"
#include "ResultTree.h"

CExportResults::CExportResults(const CExportResultData& exportResultData,
  CModelBase& model)
: m_exportResultData(exportResultData)
, m_model(model)
{
}

bool CExportResults::onExport(const QString& sFileName, bool bExcel, bool bOld,
  bool export2LAS)
{
  bool append = false;

  std::vector<const IResultExporter*> vcExporter = GetResultExporters();

  for(size_t i = 0; i < vcExporter.size(); ++i)
  {
  if(vcExporter[i])
  {
      const IResultExporter& exporter = *vcExporter[i];
      const COpenGLNode* pNode0 = exporter.GetFirstNode();

      assert(pNode0);

    IExportFormat::CExportArg arg;
    if(m_exportResultData.m_nUnit == 1) arg.m_unit = CQuantity::FIELD_UNIT;  // Get unit from control...
    if(m_exportResultData.m_nTZ == 1)	 arg.m_axis_system = IExportFormat::AXIS2D;

    // Get color scale
      arg.m_pHotSpot = m_exportResultData.m_pHotSpot;

    // Do the results
    IExportFormat::TDataVec vcResult;

      FillResultVector(vcResult, CAnalysisType::AT_LINEAR, arg.m_unit, *pNode0, true);

      if(m_exportResultData.m_bLinear)
    FillResultVector(vcResult, CAnalysisType::AT_LINEAR,  arg.m_unit, *pNode0);
    if(m_exportResultData.m_bNonLinear)
    FillResultVector(vcResult, CAnalysisType::AT_NONLIN, arg.m_unit, *pNode0);
      if(m_exportResultData.m_bHeat)
    FillResultVector(vcResult, CAnalysisType::AT_HEAT, arg.m_unit, *pNode0);
      if(m_exportResultData.m_bMixture)
    FillResultVector(vcResult, CAnalysisType::AT_MIXTURE, arg.m_unit, *pNode0);
      if(m_exportResultData.m_bMixtureContainment)
    FillResultVector(vcResult, CAnalysisType::AT_MIXTURE_CONTAINMENT, arg.m_unit, *pNode0);

      FillMatResultVector(vcResult, arg.m_unit, *pNode0);

    // Add 'normal' valueset like derived results
    for(TResultComponentSet::const_iterator it = m_exportResultData.m_RGObserverData.begin(); it != m_exportResultData.m_RGObserverData.end(); it++)
    {
      if((dynamic_cast<const IResult*>(it->first) == 0) &&
        (it->first->ModeSize() == 1) &&
        it->first->Component(it->second, 0).Defined() &&
        it->first->Component(it->second, 0).CanMap(*pNode0))
    {
        vcResult.push_back(&it->first->Component(it->second, 0));
    }
    }

      for (TResultComponentSet::const_iterator it = m_exportResultData.m_GVTRGObserverData.begin(); it != m_exportResultData.m_GVTRGObserverData.end(); it++)
      {
    if ((it->first->ModeSize() == 1) && 
          it->first->Component(it->second, 0).Defined() &&
          it->first->Component(it->second, 0).CanMap(*pNode0))
    {
          vcResult.push_back(&it->first->Component(it->second, 0));
    }
      }
      
      if (dynamic_cast<const CFormationBase*>(pNode0) ||
         dynamic_cast<const CHorizonBase*>(pNode0))
      {
      // Init deformation
      arg.m_pDeformation		= m_exportResultData.m_pTimeStep;
    arg.m_antype			= (m_exportResultData.m_nLinear == 0 ? CAnalysisType::AT_LINEAR : (m_exportResultData.m_nLinear == 1 ? CAnalysisType::AT_NONLIN : (m_exportResultData.m_nLinear == 2 ? CAnalysisType::AT_MIXTURE : CAnalysisType::AT_MIXTURE_CONTAINMENT)));
      arg.m_pResultRegister   = &m_model.ResultRegister();
    }

    if (dynamic_cast<const CFormationBase*>(pNode0) ||
      dynamic_cast<const CFormationPlane*>(pNode0) ||
          dynamic_cast<const C3DHorizon*>(pNode0))
    {
      arg.m_elementCenterPoints = (m_exportResultData.m_nCenterPoints == 0 ?
          IExportFormat::FULL_ELEMENT :
      (m_exportResultData.m_nCenterPoints == 1 ?
              IExportFormat::CENTER_ELEMENT : IExportFormat::CENTER_WELL));
    }

      if(dynamic_cast<const CFormationBase*>(pNode0) ||
         dynamic_cast<const CPointSet*>(pNode0)  ||
         dynamic_cast<const CNewWellPath*>(pNode0))
      {
    arg.m_bFormationNames = (m_exportResultData.m_nFormationNames != 0);
      }

    // Extract weighting type for element set
    if(dynamic_cast<const IElementSet*>(pNode0))
      {
      switch(m_exportResultData.m_nWeightingType)
      {
      case 0:
        arg.m_map_type = CValueType::MT_POINT;
        break;
      case 1:
        arg.m_map_type = CValueType::MT_NONE;		// no weighting
        break;
      case 2:
        arg.m_map_type = CValueType::MT_VOIGHT;	// volume weighted
        break;
      case 3:
        arg.m_map_type = CValueType::MT_REUSS;
        break;
      default:
        assert(false);
        break;
      }
    }

      arg.m_wellpathPoints = m_exportResultData.m_nWellPoints == 0 ? IExportFormat::INPUT_POINTS : IExportFormat::MESH_INTERSECTION_POINTS;

    // set format type
    arg.m_bOldFormat = bOld;

    if( bExcel ) exporter.ExportExcel(sFileName, vcResult, arg, append);
      else if (export2LAS)
      {
    exporter.Export2LAS(sFileName, vcResult, arg);
      }
      else         exporter.ExportASCII(sFileName, vcResult, arg, append);

      append = true;
  }
  }

  return true;

}

// static

bool CExportResults::isInputResult(const IResult* result)
{
  const CResultTree::CScalarResult* scalarResult =
  dynamic_cast <const CResultTree::CScalarResult*> (result);

  return (scalarResult != 0) && (scalarResult->Parent() != 0) &&
  (scalarResult->Parent()->Parent() != 0) &&
  ((scalarResult->Parent()->Parent()->Name() ==
      getStringTableEntry(IDS_RG_INPUT)) ||
      ((scalarResult->Parent()->Parent()->Parent() != 0) &&
      (scalarResult->Parent()->Parent()->Parent()->Name() ==
    getStringTableEntry(IDS_RG_INPUT))));
}

// private

void CExportResults::FillResultVector(IExportFormat::TDataVec& vcRet,
  const CAnalysisType& antype, CQuantity::UNIT /*unit*/,
  const COpenGLNode& node, bool fillInput)
{
  // Walk over timesteps
  const CDepletionStage* pStage = &m_model.InitialDepletionStage();
  while(pStage)
  {
    // Walk over results
    for(TResultComponentSet::const_iterator it = m_exportResultData.m_RGObserverData.begin(); it != m_exportResultData.m_RGObserverData.end(); it++)
    {
      const IResult *pResult = dynamic_cast<const IResult*>(it->first);
      const CResultTree::CScalarResult* scalarResult =
    dynamic_cast <const CResultTree::CScalarResult*> (pResult);
      bool inputResult = CExportResults::isInputResult(scalarResult);

      if(pResult && (fillInput == inputResult) && (m_exportResultData.m_stTimeStep.find(pStage) != m_exportResultData.m_stTimeStep.end()))
      {
        int nMode = pResult->Mode( antype, *pStage );
        
        if(nMode >= 0 && pResult->Component(it->second, nMode).Defined() &&
          pResult->Component(it->second, nMode).
          CanMap(node))
          vcRet.push_back(&pResult->Component(it->second, nMode));
      }
    }

    if(pStage->Last())
      pStage = 0;
    else
      pStage = &pStage->Next();
  }
}

void CExportResults::FillMatResultVector(IExportFormat::TDataVec& vcRet,
  CQuantity::UNIT /*unit*/, const COpenGLNode& node)
{
  // Walk over timesteps
  const CDepletionStage* pStage = &m_model.InitialDepletionStage();
  while(pStage)
  {
    // Walk over results
    for(TResultComponentSet::const_iterator it = m_exportResultData.m_MatRGObserverData.begin(); it != m_exportResultData.m_MatRGObserverData.end(); it++)
    {
      const IResult *pResult = dynamic_cast<const IResult*>(it->first);
      if(pResult && (m_exportResultData.m_stTimeStep.find(pStage) != m_exportResultData.m_stTimeStep.end()))
      {
        if(pResult->ResultComponent(*pStage, CAnalysisType::AT_LINEAR)->Defined() &&
          pResult->ResultComponent(*pStage, CAnalysisType::AT_LINEAR)->
          CanMap(node))
          vcRet.push_back(pResult->ResultComponent(*pStage, CAnalysisType::AT_LINEAR));
      }
    }

    if(pStage->Last())
      pStage = 0;
    else
      pStage = &pStage->Next();
  }
}

std::vector<const IResultExporter*> CExportResults::GetResultExporters()
{
  COpenGLNodeSelection& sel(m_exportResultData.m_selection);
  std::vector<const IResultExporter*> vcRet;
  CExporterFactory fac(m_model);

  THorizonBaseSelection* sliphors = new THorizonBaseSelection(sel, new CSlipHorizonSelectionFilter);
  vcRet.push_back(fac.CreateExporter(*sliphors));

  THorizonBaseSelection* hors = new THorizonBaseSelection(sel, new CHorizonSelectionFilter);
  vcRet.push_back(fac.CreateExporter(*hors));

  TFormationSelection* forms = new TFormationSelection(sel);
  vcRet.push_back(fac.CreateExporter(*forms));

  TFormationPlaneSelection* planes = new TFormationPlaneSelection(sel);
  vcRet.push_back(fac.CreateExporter(*planes));

  TRTCISelection* rtci = new TRTCISelection(sel);
  vcRet.push_back(fac.CreateExporter(*rtci));

  TNewWellPathSelection* nwp = new TNewWellPathSelection(sel);
  vcRet.push_back(fac.CreateExporter(*nwp));

  TPointSetSelection* ps = new TPointSetSelection(sel);
  vcRet.push_back(fac.CreateExporter(*ps));

  TElementSetSelection* es = new TElementSetSelection(sel, new CElementSetSelectionFilter);
  vcRet.push_back(fac.CreateExporter(*es));

  TNonMeshedSurfaceSelection* nms = new TNonMeshedSurfaceSelection(sel);
  vcRet.push_back(fac.CreateExporter(*nms));

  return vcRet;
}
