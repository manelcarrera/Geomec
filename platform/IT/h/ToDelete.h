#pragma once

// TFormationBaseEntry::TNodeSet formations = static_cast< TFormationBaseEntry* >( model.GraphEntry( MD_BASE_MESH_RESULT
// ) )->EntryNodes();

// GVTData
/*CPointSet* m_InputPointSet = new CPointSet(gridDef.Name() + "_input", model, nValueSize, IPointSet::DIM_3D);
//m_Input.FillPointSet(progress, *m_PillarMap, m_InputPointSet);
TPointSetEntry& psentry = (TPointSetEntry&)(model.GraphEntry( MD_BASE_POINTSET ) );
psentry.ConnectItem(*m_InputPointSet);*/

// GVTData
/*
TPointSetEntry::TNodeSet stPsNode = ((TPointSetEntry*)Model().GraphEntry(MD_BASE_POINTSET))->EntryNodes();
TPointSetEntry::TNodeSet::iterator it;
for(it = stPsNode.begin(); it != stPsNode.end(); ++it)
{
    CPointSet* pPointSet = dynamic_cast<CPointSet*>(*it);
    if(pPointSet && (pPointSet->Name() == pointSetName))
    {
  if(bReinterpolate)
  {
        CopyPointSet(pointSetName, *pPointSet, progress);
        delete pPointSet;
  }
  else
  {// just use this pointset, it should still be good.
        m_AnalysisPointSet = pPointSet;
  }
  return;
    }
}
*/

/*void CGVTData::AddPoint(const geo::IPoint &point)
{
assert(m_AnalysisPointSet);
std::vector<geo::CValue> vcPointData(3);
vcPointData[0] = point.X();
vcPointData[1] = point.Y();
vcPointData[2] = point.Z();
m_AnalysisPointSet->PushBack(vcPointData);
*/

// FormationBase.cpp
/*	CDepletionStageEntry& entry = (CDepletionStageEntry&)(*Model().GraphEntry(MD_BASE_DEPLETION_STAGE));
assert(IsLinkedTo(entry));
if(&entry == &node && !model.Loading())
{
CDepletionStageEntry::iterator previous = entry.end();

for(CDepletionStageEntry::iterator itd = entry.begin(); itd != entry.end(); ++itd)
  {
    CPressure *pPressure = 0;

    if(m_mpPressure.find(&*itd) == m_mpPressure.end())
    {
      // Create new one ...
      pPressure = new CPressure(*this, *itd);	// Default "Same as previous"



CDepletionStage *pStage = &((CModelBase&)Model()).InitialDepletionStage();


// Link to depletionstage entry
LinkTo(*Model().GraphEntry(MD_BASE_DEPLETION_STAGE));


      */

/*void ITensorGroup::CVectorComposite::BuildComponent(const CDepletionStage& stage, const CAnalysisType& antype, int
nRegister)
{
if(OnBuildComponent(stage, antype, nRegister))
{
//new CFullVectorComponent(*this, stage, antype, nRegister);
  new CVectorComponent(IDS_RC_NORTHING, *this, stage, antype, nRegister, VC_X);
  new CVectorComponent(IDS_RC_EASTING, *this, stage, antype, nRegister, VC_Y);
  new CVectorComponent(IDS_RC_DEPTH, *this, stage, antype, nRegister, VC_Z);
  new CLengthComponent(*this, stage, antype, nRegister);
new CFullVectorComponent(*this, stage, antype, nRegister);
}
}*/

// CModelTreeView* tree = GetModelTreeView();
// CModelView* view = GetModelView();

// CModelBase& model = ((CModelBase&)*Model()).RootModel();

// Dlg dlg( model );
// dlg.OnExport1();

// CResultExportDlg dlg( model, _cb1 );
// dlg.show();
// dlg.OnExport( "fake.txt", false, false, _cb1 );

// CExportResultData exportResultData( model );

// getExportResultData(&exportResultData, false);
// CExportResults exportResults(exportResultData, m_model);

// #include "ExportResultData.h"

#ifdef GG
void getExportResultData(CExportResultData *exportResultData) {
  // CString macroName;

  // UpdateData(TRUE);

  // m_macroName.GetWindowText(macroName);
  // exportResultData->Name((LPCTSTR) macroName);
  // exportResultData->m_selection = Selection();
  // exportResultData->m_nUnit = m_nUnit;
  // exportResultData->m_nTZ = m_nTZ;

  // exportResultData->m_pHotSpot = (const CHotSpot*) m_cbHotSpot.GetItemData(m_cbHotSpot.GetCurSel());
  exportResultData->m_bLinear = false;
  exportResultData->m_bNonLinear = true;
  // exportResultData->m_bHeat = m_bHeat;
  // exportResultData->m_bMixture = m_bMixture;
  // exportResultData->m_bMixtureContainment = m_bMixtureContainment;

  m_RGObserver = QSharedPointer<CExportResultGroupObserver>(new CExportResultGroupObserver(
      *this, m_model.ResultTree(), m_tcValue,
      (m_exportResultData == 0 ? TResultComponentSet() : m_exportResultData->m_RGObserverData)));

  exportResultData->m_RGObserverData = m_RGObserver->Results();

  exportResultData->m_pTimeStep = (const CDepletionStage *)m_cbTimeStep.GetItemData(m_cbTimeStep.GetCurSel());
  /*exportResultData->m_nLinear = m_nLinear;
  exportResultData->m_nCenterPoints = m_nCenterPoints;
  exportResultData->m_nWellPoints = m_nWellPoints;
  exportResultData->m_nFormationNames = m_nFormationNames;
  exportResultData->m_nWeightingType = m_nWeightingType;
  exportResultData->m_stTimeStep = m_stTimeStep;*/
  exportResultData->m_MatRGObserverData = m_MatRGObserver->Results();
}
#endif

/*#include "ResultExportDlg.h"

void _cb1( IExportFormat::TDataVec _v )
{
  int i=0;
}*/

/*class Dlg : public CResultExportDlg
{
public:
  Dlg( CModelBase& model ) : CResultExportDlg( model ){}
  bool OnExport1(	const CString& sFileName = "fake.txt",
          bool bExcel = false,
          bool bOld = false,
          void (*_cb)( IExportFormat::TDataVec ) = _cb1 ){
            return OnExport( sFileName , bExcel, bOld, _cb );	}
};*/

#include "PyWrapper.h"
typedef PyWrapper wrapper;

#ifdef BB
class Foo {
public:
  CModelBase &model;

  Foo(CModelBase &_model) : model(_model) {}

  std::list<std::string> get_formations_list() {
    // return std::list< std::string >();
    std::list<std::string> formations_list;

    TFormationBaseEntry::TNodeSet formations =
        static_cast<TFormationBaseEntry *>(model.GraphEntry(MD_BASE_FORMATION))->EntryNodes();
    for (TFormationBaseEntry::TNodeSet::iterator it = formations.begin(); it != formations.end(); ++it) {
      CFormationBase &formation = dynamic_cast<CFormationBase &>(**it);
      formations_list.push_back(formation.Name().toStdString()); // TODO: verify
    }

    return formations_list;
  }

  void get_formation_data(const std::string _name) {
    // points ; x, y z
    // elems : nodes : triangles / hexa

    TFormationBaseEntry::TNodeSet formations =
        static_cast<TFormationBaseEntry *>(model.GraphEntry(MD_BASE_FORMATION))->EntryNodes();
    for (TFormationBaseEntry::TNodeSet::iterator it = formations.begin(); it != formations.end(); ++it) {
      CFormationBase &formation = dynamic_cast<CFormationBase &>(**it);
      std::string name = formation.Name().toStdString();

      if (name.compare(_name) == 0) {
        /*const IExportFormat::TDataVec vcNodalData;
        const IExportFormat::TDataVec vcElementData;
        const IExportFormat::CExportArg arg;

        CElementSetExportFormat helper(	formation, vcNodalData, vcElementData, arg );*/

        const int POS_POINT_ID = 0; // FIXME
        const int POS_X = 1;
        const int POS_Y = 2;
        const int POS_Z = 3;

        IFormationElementSet &es1 = formation.ElementSet(0);
        geo::IElementSet &es2 = es1.ElementSet();

        const int POINTS_COLS = 4;     // FXIME
        const int POINTS_PER_ELEM = 4; // FIXME

        int nElems = es2.ElementSize();
        int nPointsPerElem = es2.Element(0).NrOfPoints();

        const int &ref = nElems * nPointsPerElem;
        // const int POINTS_ROWS = const_cast< int& >( ref );

        const int _NUM_POINTS = 100 * 1000; // FIXME
        const int _NUM_ELEMS = 10 * 1000;   // FIXME

        long double (*points)[POINTS_COLS] =
            wrapper::_create_array<long double (*)[POINTS_COLS], _NUM_POINTS, POINTS_COLS>();
        long double (*elems)[POINTS_PER_ELEM] =
            wrapper::_create_array<long double (*)[POINTS_PER_ELEM], _NUM_ELEMS, POINTS_PER_ELEM>();

        int idx = 0;

        for (int e = 0; e < es2.ElementSize(); e++) // elems
        {
          const geo::IElement &element = es2.Element(e);

          for (int p = 0; p < element.NrOfPoints(); p++) // point per elem
          {
            points[idx][POS_POINT_ID] = (double)element.PointIndex(p);
            points[idx][POS_X] = element.Point(p).X();
            points[idx][POS_Y] = element.Point(p).Y();
            points[idx][POS_Z] = element.Point(p).Z();

            elems[e][p] = (double)element.PointIndex(p);

            idx++;
          }
        }
        int a = 1;

        for (int i = 0; i < idx; i++) // all the points
        {
          Printer::_print_format("%.0f\t%E\t%E\t%E", points[i][POS_POINT_ID], points[i][POS_X], points[i][POS_Y],
                                 points[i][POS_Z]);
        }
        a = 2;
        for (int e = 0; e < es2.ElementSize(); e++) // elems
        {
          Printer::_print_format("%.0f %.0f %.0f %.0f", elems[e][0], elems[e][1], elems[e][2], elems[e][3]);
        }
        a = 3;
      }
    }
  }
};
#endif
