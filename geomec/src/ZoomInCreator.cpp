#include "ZoomInCreator.h"
#include "BaseEntryTypes.h"
#include "BoundaryInterfaceDef.h"
#include "ElementValueSet.h"
#include "FemAppMainWindow.h"
#include "FormationBase.h"
#include "GUIBase.h"
#include "GeoSurface.h"
#include "GeomecUtils.h"
#include "GlobalInitialStressNode.h"
#include "GlobalMessage.h"
#include "HexaGUI.h"
#include "HexaModel.h"
#include "HomogenizationBox.h"
#include "HomogenizationBox_Delegate.h"
#include "ISceneWrapper.h"
#include "MeshBase.h"
#include "NodalValueSet.h"
#include "ResultRegister.h"
#include "ResultTree.h"
#include "SpringElement.h"
#include "TetraBoundary.h"
#include "TetraEntryTypes.h"
#include "TetraFormation.h"
#include "TetraModel.h"
#include "TetraSubBoundary.h"
#include "ValueTensor.h"
#include "ValueTypeFactory.h"
#include "ValueTypes.h"
#include "ValueVector.h"
#include "ddxquantity.h"
#include "geomec.h"
#include "geoprogress.h"
#include "keyfile.h"
#include "materialserver.h"
#include "modelbase.h"
#include "resultexportdlg.h"
#include "stdafx.h"
#include "tnofiledialog.h"

/* Zoomincreator implementation		////////////////////////////////////////////*/
CZoomInCreator::CZoomInCreator(CHomogenizationBox &box) : m_box(&box) {}

void CZoomInCreator::ExtractResults(CElementSet &element_set, const CAnalysisType &antype) {
  CModelBase &model = (CModelBase &)m_box->Model();
  const CDepletionStage *pStage = &model.InitialDepletionStage();
  while (pStage) {
    ExtractResults(element_set, *pStage, antype);
    if (pStage->Last())
      pStage = 0;
    else
      pStage = &pStage->Next();
  };
}

/*!
  Functions maps result components on ElementValuesSets.
  The ElementValueSets are created in the function.
*/
void CZoomInCreator::MapResults(const QString &sResultName, const IResult &source, CValueType &target,
                                TResultMap &result_map, const CDepletionStage &stage, const CAnalysisType &antype) {
  CString sAnalysis;
  sAnalysis.Format("_D%d_", stage.Index());
  sAnalysis += antype.ExportCharacter();

  for (unsigned int i = 0; i < target.ComponentSize(); i++) {
    const IResultComponent *result_component = source.ResultComponent(stage, antype, 0, i);
    if (result_component) {
      IValueComponentBase &component = target.Component(i);
      QString sName = sResultName + component.Name() + sAnalysis;
      CElementSet &element_set = (CElementSet &)target.PointSet();
      CElementValueSet *pValueSet = new CElementValueSet(sName, CQuantity::SI_UNIT, element_set);
      pValueSet->LinkTo(component);
      result_map.push_back(TResultPair(result_component, pValueSet));
    }
  }
}

/*!
  Results are extracted for every depletionstage.
*/
void CZoomInCreator::ExtractResults(CElementSet &element_set, const CDepletionStage &stage,
                                    const CAnalysisType &antype) {
  QString sAnalysis;
  sAnalysis = QString("D%1_").arg(stage.Index());
  sAnalysis += antype.ExportCharacter();

  // We create a result pair vector of results component and CElementValueSets
  // and later we will put the results in the elementset.
  CModelBase &model = (CModelBase &)m_box->Model();
  TResultMap mpResult;
  // Do the stress tensor ...
  TStressTensor *pTensor = new TStressTensor(element_set, sAnalysis);
  MapResults("Stress", model.ResultTree().TotalStress().Components(), *pTensor, mpResult, stage, antype);
  if (!stage.Initial()) {
    TDisplacementVector *pVector = new TDisplacementVector(element_set, sAnalysis);
    MapResults("Displacement", model.ResultTree().Displacement(), *pVector, mpResult, stage, antype);
  }

  // Process results
  for (size_t i = 0; i < mpResult.size(); i++) {
    const IResultComponent &component = *mpResult[i].first;
    CElementValueSet &value_set = *mpResult[i].second;
    assert(value_set.ValueSize() == 0);
    for (int j = 0; j < element_set.ElementSet().ElementSize(); j++) {
      const geo::IElement &element = element_set.ElementSet().Element(j);
      IValueDomainScalar::TValueVec vcValue = component.ValueElement(element);
      value_set.PushBack(vcValue);
    }
    assert(value_set.ValueSize() == element_set.ElementSet().ElementSize());
  }
}

void CZoomInCreator::Create() {
  CGUIBase *model = dynamic_cast<CGUIBase *>(GetGeomecDoc()->GUI());
  //  *************************************************************************
  //  Save the global model first
  //  *************************************************************************
  if (GetGeomecDoc()->IsModified()) {
    if (_m()->msg("Please save your changes first.", MB_ICONEXCLAMATION | MB_OKCANCEL) == IDOK)
      GetGeomecDoc()->OnSaveDocument(GetGeomecDoc()->GetPathName());
    else
      return;
  }

  // now "Save as" the zoom in model
  QString sFilter(getStringTableEntry(IDS_FILE_SAVE_FILTER));
  CTnoFileDialog dlg(FALSE, GEOMEC_DEFAULT_EXTENSION, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
                     sFilter.toStdString().c_str(), FemAppGetMainWnd());

  if (dlg.DoModal() == IDOK) {

    // Do the dialog for the FAR field.
    CSinglePressure shear(10E6, 0.0, DOUBLE_MAX_, true, true);

    // throw away all the boxes except the box that is used to create the zoom in model
    CHomoBoxEntry::TNodeSet box_nodes = ((CHomoBoxEntry *)model->model()->GraphEntry(MD_BASE_HOMO_BOX))->EntryNodes();
    CHomoBoxEntry::TNodeSet::iterator it_box;
    for (it_box = box_nodes.begin(); it_box != box_nodes.end(); it_box++) {
      if (*it_box == m_box)
        continue;
      (**it_box).Destroy();
    }

    // Copy stress and displacement results from the globalmodel
    CElementSet *pResultSet = m_box->CaptureElementSet(*model->model());
    if (model->model()->ResultRegister().Linear())
      ExtractResults(*pResultSet, CAnalysisType::AT_LINEAR);
    if (model->model()->ResultRegister().NonLinear())
      ExtractResults(*pResultSet, CAnalysisType::AT_NONLIN);

    if (GetGeomecDoc()->CurrentScene()) {
      GetGeomecDoc()->CurrentScene()->Clear();
      if (m_box)
        GetGeomecDoc()->CurrentScene()->LinkTo(*m_box);
    }

    // Create zoom - in model here.... ///////////////////////////////////////////////////////////////
    CHexaGUI *hexa = dynamic_cast<CHexaGUI *>(model);
    if (hexa) {
      model = hexa->ConvertToTetra();

      // from now on the m_box is f*cked up!!
      // m_box = 0;
      // wjrx improved zoom-in mantis 2421: refetch m_box
      //
      assert(((CHomoBoxEntry *)model->model()->GraphEntry(MD_BASE_HOMO_BOX))->EntryNodes().size() == 1);
      m_box = *(((CHomoBoxEntry *)model->model()->GraphEntry(MD_BASE_HOMO_BOX))->EntryNodes().begin());
    }
    model->model()->Mesh().InvalidateMesh();

    // from here we're always dealing with a tetramodel
    CTetraModel &tetra = dynamic_cast<CTetraModel &>(*model->model());
    assert(&tetra);

    tetra.GlobalInitialStress().WaterColumnStressGradient(0.0);

    CTetraBoundary &tb = (CTetraBoundary &)(tetra.Boundary());
    tb.InterfaceDefinition().Shear(shear.Value(CDoubleQuantity::SI_UNIT), CDoubleQuantity::SI_UNIT);
    tb.InterfaceDefinition().KtanTop(tb.InterfaceDefinition().Ktan(CDoubleQuantity::SI_UNIT), CDoubleQuantity::SI_UNIT);
    tb.InterfaceDefinition().KtanBottom(tb.InterfaceDefinition().Ktan(CDoubleQuantity::SI_UNIT),
                                        CDoubleQuantity::SI_UNIT);
    tb.InterfaceDefinition().KtanSides(tb.InterfaceDefinition().Ktan(CDoubleQuantity::SI_UNIT),
                                       CDoubleQuantity::SI_UNIT);
    tb.InterfaceDefinition().KradTop(tb.InterfaceDefinition().Krad(CDoubleQuantity::SI_UNIT), CDoubleQuantity::SI_UNIT);
    tb.InterfaceDefinition().KradBottom(tb.InterfaceDefinition().Krad(CDoubleQuantity::SI_UNIT),
                                        CDoubleQuantity::SI_UNIT);
    tb.InterfaceDefinition().KradSides(tb.InterfaceDefinition().Krad(CDoubleQuantity::SI_UNIT),
                                       CDoubleQuantity::SI_UNIT);
    tb.CreateInterfaces(TRUE);
    tb.InterfaceDefinition().IntMatType(CBoundaryInterfaceDef::G);

    tetra.GraphEntry(MD_TETRA_SUB_HORIZON)->Destroy();
    tetra.GraphEntry(MD_TETRA_SUB_FAULT)->Destroy();
    tetra.GraphEntry(MD_TETRA_SUPER_HORIZON)->Destroy();
    tetra.GraphEntry(MD_BASE_XSECTION)->Destroy();
    tetra.GraphEntry(MD_BASE_WELLPATH)->Destroy();

    // before we can remove the formations, we first have to remove the volumes in the formations
    CTetraFormationEntry::TNodeSet nodes = ((CTetraFormationEntry *)tetra.GraphEntry(MD_TETRA_FORMATION))->EntryNodes();
    CTetraFormationEntry::TNodeSet::iterator it;
    for (it = nodes.begin(); it != nodes.end(); it++) {
      CTetraFormation &format = **it;
      int nVolSize = format.VolumeSize();
      while (nVolSize) {
        format.UnLink(format.Volume(0));
        nVolSize = format.VolumeSize();
      }
    }
    // now destroy the formations
    tetra.GraphEntry(MD_TETRA_FORMATION)->Destroy();

    CTetraSubBoundary &sub_bound = tetra.SubBoundary();
    int nSideSize = sub_bound.SideSurfaceSize();
    while (nSideSize) {
      sub_bound.UnLink(sub_bound.SideSurface(0));
      nSideSize = sub_bound.SideSurfaceSize();
    }

    COleDateTime time = COleDateTime::GetCurrentTime();
    QString info;
    info = QString("Extracted from [" + tetra.ProjectInfo() + "] on " + time.Format(VAR_DATEVALUEONLY));
    tetra.ProjectInfo(info);

    //////////////////////////////////////////////////////////////////////////////////////////////////

    // Finally save all the changes
    GetGeomecDoc()->OnSaveDocument(dlg.GetPathName());

    // wjrx improved zoom-in mantis 2421
    // after creation of zoom in model,
    // directly display cut 'surafces' menu
    //
    if (m_box != 0) {
      CHomogenizationBox_Delegate *m_box_Delegate = dynamic_cast<CHomogenizationBox_Delegate *>(m_box->getDelegate());

      if (m_box_Delegate->CanCutSurafceInDatastoreWithBox()) {
        m_box_Delegate->CutSurafceInDatastoreWithBox();
      }
    }
  }
}
