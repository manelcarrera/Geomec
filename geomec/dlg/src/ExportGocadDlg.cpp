// ExportGocadDlg.cpp: implementation of the CExportGocadDlg class.
//
//////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "Geomec.h"

#include "ExportGocadDlg.h"
#include "DepletionStage.h"
#include "ModelBase.h"
#include "ResultTree.h"
#include "FormationBase.h"
#include "BaseEntryTypes.h"
#include "SelectionObserver.h"
#include "TnoFileDialog.h"
#include "HorizonBase.h"
#include "TetraEntryTypes.h"
#include "TetraSubHorizon.h"
#include "PointSet.h"
#include "SelectionObserver_Delegate.h"
#include "PointSet_Delegate.h"
#include "TetraSubHorizon_Delegate.h"
#include "FormationBase_Delegate.h"
#include "GeomecUtils.h"
#include "PointSetEntryObserver.h"
#include "DepletionStageObserver.h"
#include "MaterialResultTree.h"
#include "GVTResult.h"
#include "GlobalMessage.h"

////

CExportGocadDlg::CObjectSelection::CObjectSelection(CExportGocadDlg& dlg)
: m_dlg(dlg)
{
}

void CExportGocadDlg::CObjectSelection::populateTree()
{
  { // formations
    TFormationBaseEntry& entry = (TFormationBaseEntry&)(*m_dlg.Model().GraphEntry(MD_BASE_FORMATION));
    typedef CSelectionLeafObserver_Delegate<CFormationBase, CFormationBase_Delegate, CObjectSelection, FALSE> TLeafObs;
    typedef CSelectionBranchObserver_Delegate<TFormationBaseEntry, TFormationBaseEntry_Delegate, CFormationBase, TLeafObs, CFormationBase, CObjectSelection, FALSE, FIXED_ITEM> TEntryObs;
    new TEntryObs(entry, m_dlg.m_tcObjects, *this, &CObjectSelection::formation_selected, &CObjectSelection::select_formation);
  }

  { // faults
    if(m_dlg.Model().GraphEntry(MD_TETRA_SUB_FAULT) != 0) // must be a tetra model
    {
      CTetraSubHorizonEntry& entry = (CTetraSubHorizonEntry&)(*m_dlg.Model().GraphEntry(MD_TETRA_SUB_FAULT));
      typedef CSelectionLeafObserver_Delegate<CTetraSubHorizon, CTetraSubHorizon_Delegate, CObjectSelection, FALSE> TLeafObs;
      typedef CSelectionBranchObserver_Delegate<CTetraSubHorizonEntry, CTetraSubHorizonEntry_Delegate, CTetraSubHorizon, TLeafObs, CTetraSubHorizon, CObjectSelection, FALSE, FIXED_ITEM> TEntryObs;

      new TEntryObs(entry, m_dlg.m_tcObjects, *this, &CObjectSelection::fault_selected, &CObjectSelection::select_fault);
    }
  }

  { // pointsets
    TPointSetEntry& entry = (TPointSetEntry&)(*m_dlg.Model().GraphEntry(MD_BASE_POINTSET));
    typedef CSelectionLeafObserver_Delegate<CPointSet, CPointSet_Delegate, CObjectSelection, FALSE> TLeafObs;
    typedef CSelectionBranchObserver_Delegate<TPointSetEntry, TPointSetEntry_Delegate, CPointSet, TLeafObs, CPointSet, CObjectSelection, FALSE, FIXED_ITEM> TEntryObs;
    new TEntryObs(entry, m_dlg.m_tcObjects, *this, &CObjectSelection::pointset_selected, &CObjectSelection::select_pointset);
  }
}

void CExportGocadDlg::CObjectSelection::select_formation(const CFormationBase& formation, bool bSelect)
{
  m_dlg.Formation(formation, bSelect);
}

bool CExportGocadDlg::CObjectSelection::formation_selected(const CFormationBase& formation) const
{
  return m_dlg.Formation(formation);
}

void CExportGocadDlg::CObjectSelection::select_fault(const CTetraSubHorizon& fault, bool bSelect)
{
  m_dlg.Fault(fault, bSelect);
}

bool CExportGocadDlg::CObjectSelection::fault_selected(const CTetraSubHorizon& fault) const
{
  return m_dlg.Fault(fault);
}

void CExportGocadDlg::CObjectSelection::select_pointset(const CPointSet& ptSet, bool bSelect)
{
  return m_dlg.PointSet(ptSet, bSelect);
}

bool CExportGocadDlg::CObjectSelection::pointset_selected(const CPointSet& ptSet) const
{
  return m_dlg.PointSet(ptSet);
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CExportGocadDlg, CDialog)
  ON_BN_CLICKED(IDC_BUTTON_EXPORT, OnExport)
  ON_BN_CLICKED(IDC_CHECK_LINEAR, OnAnalysisTypeChanged)
  ON_BN_CLICKED(IDC_CHECK_NONLIN, OnAnalysisTypeChanged)
  ON_BN_CLICKED(IDC_CHECK_HEAT, OnAnalysisTypeChanged)
  ON_BN_CLICKED(IDC_CHECK_MIXTURE, OnAnalysisTypeChanged)
END_MESSAGE_MAP()

CExportGocadDlg::CExportGocadDlg(CGocadExport& gocadexport, const CResultGroup* pResultGroup, CWnd* pParent)
: IExportDlg(IDD_EXPORT_GOCAD, pParent),
  m_source(gocadexport),
  m_pCopy(new CGocadExport(gocadexport)),
  m_current(0),
  m_MatRGObserver(0),
  m_RGObserver(0),
  m_GVTRGObserver(0),
  m_InitiallyOn(pResultGroup)
{
  CollectDepletionStages();
}

CExportGocadDlg::~CExportGocadDlg()
{
  if(m_current)
    delete m_current;

  delete m_pCopy;
}

void CExportGocadDlg::CollectDepletionStages()
{
	const CDepletionStage* pStage = &m_source.Model().InitialDepletionStage();
	while(pStage)
	{
    m_pCopy->AddDepletionStage(*pStage);

		if(pStage->Last())
			pStage = 0;
		else
			pStage = &pStage->Next();
	}
}

void CExportGocadDlg::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);

  int iLinear;
  int iNonLinear;
  int iHeat;
  int iMixture;
  int iUnit;
  int iVectorESize;
  int iTensorESize;
  int iAverage;
  int iSplitFormations;
  int iFormationNames;

  if(!pDX->m_bSaveAndValidate)
  {
    iLinear          = m_pCopy->Linear() ? 1 : 0;
    iNonLinear       = m_pCopy->NonLinear() ? 1 : 0;
    iHeat            = m_pCopy->Heat() ? 1 : 0;
    iMixture         = m_pCopy->Mixture() ? 1 : 0;
    iUnit            = m_pCopy->Unit();
    iVectorESize     = m_pCopy->VectorESize() ? 1 : 0;
    iTensorESize     = m_pCopy->TensorESize() ? 1 : 0;
    iAverage         = m_pCopy->Average() ? 1 : 0;
    iSplitFormations = m_pCopy->SplitFormations() ? 1 : 0;
    iFormationNames  = m_pCopy->FormationNames() ? 1 : 0;
  }

	DDX_Control(pDX, IDC_LIST_STAGES, m_lbTimeStep);
	DDX_Control(pDX, IDC_TREE_RESULTS, m_tcResults);
  DDX_Control(pDX, IDC_TREE_FORMATIONS, m_tcObjects);

  DDX_Check(pDX, IDC_CHECK_LINEAR,  iLinear);
  DDX_Check(pDX, IDC_CHECK_NONLIN,  iNonLinear);
  DDX_Check(pDX, IDC_CHECK_HEAT,    iHeat);
  DDX_Check(pDX, IDC_CHECK_MIXTURE, iMixture);

  DDX_Radio(pDX, IDC_RADIO_SI, iUnit);

  DDX_Check(pDX, IDC_CHECK_VECTOR_ESIZE, iVectorESize);
  DDX_Check(pDX, IDC_CHECK_TENSOR_ESIZE, iTensorESize);
  DDX_Check(pDX, IDC_CHECK_AVERAGE, iAverage);
  DDX_Check(pDX, IDC_CHECK_SPLITFORMATIONS, iSplitFormations);
  DDX_Check(pDX, IDC_FORMATION_NAMES, iFormationNames);

  if(pDX->m_bSaveAndValidate)
  {
    m_pCopy->Linear(iLinear != 0);
    m_pCopy->NonLinear(iNonLinear != 0);
    m_pCopy->Heat(iHeat != 0);
    m_pCopy->Mixture(iMixture != 0);
    m_pCopy->Unit(iUnit);
    m_pCopy->VectorESize(iVectorESize != 0);
    m_pCopy->TensorESize(iTensorESize != 0);
    m_pCopy->Average(iAverage != 0);
    m_pCopy->SplitFormations(iSplitFormations != 0);
    m_pCopy->FormationNames(iFormationNames != 0);
    m_pCopy->MaterialParameters() = m_MatRGObserver->Results();
    m_pCopy->Results() = m_RGObserver->Results();
  }
}

BOOL CExportGocadDlg::OnInitDialog()
{
  CDialog::OnInitDialog();

	UpdateTimeStep();

  // add formations and expand
  m_tcObjects.DeleteAllItems();
  m_current = new CObjectSelection(*this);
  m_current->populateTree();
  HTREEITEM hRoot = m_tcObjects.GetRootItem();
  while(hRoot != 0)
  {
    m_tcObjects.Expand(hRoot, TVE_EXPAND);
    hRoot = m_tcObjects.GetNextSiblingItem(hRoot);
  }

  m_MatRGObserver = new CLocalResultGroupObserver<CExportGocadDlg>(*this, m_source.Model().MaterialResultTree(), m_tcResults);
  m_GVTRGObserver = new CLocalResultGroupObserver<CExportGocadDlg>(*this, m_source.Model().GVTResultTree(), m_tcResults);
  m_RGObserver = new CLocalResultGroupObserver<CExportGocadDlg>(*this, m_source.Model().ResultTree(), m_tcResults);

  typedef CSelectionLeafObserver_Delegate<IValueComposite, IValueComposite_Delegate, CExportGocadDlg, FALSE> TCompositeObs;
	typedef CSelectionBranchObserver_Delegate<CGraphEntry, CGraphEntry_Delegate, IValueComposite, TCompositeObs, IValueComposite, CExportGocadDlg, FALSE, FIXED_ITEM> TCompositeEntryObs;

	// Insert resulttree
	new TCompositeEntryObs(*Model().GraphEntry(MD_BASE_MESH_RESULT), m_tcResults, *this,
					       &CExportGocadDlg::ResultComposite, &CExportGocadDlg::ResultComposite);

	m_tcResults.UpdateTree();

  return TRUE;
}

void CExportGocadDlg::OnExport()
{
	if(!m_pCopy || (!m_pCopy->HasFormations() && !m_pCopy->HasHorizons() && !m_pCopy->HasPointSets()))
	{
		_m()->msg(IDS_NO_OBJECTS_SELECTED);
		return;
	}

  CString sFilter;
  CString sDefExt;
  if(!m_pCopy->HasHorizons() && !m_pCopy->HasPointSets())
  {
    sFilter = "Gocad Solid Files (*.so)|*.so|All Files (*.*)|*.*|";
    sDefExt = "so";
  }
  else if(!m_pCopy->HasFormations() && !m_pCopy->HasPointSets())
  {
    sFilter = "Gocad Surface Files (*.ts)|*.ts|All Files (*.*)|*.*|";
    sDefExt = "ts";
  }
  else if(!m_pCopy->HasFormations() && !m_pCopy->HasHorizons())
  {
    sFilter = "Gocad Pointset Files (*.vs)|*.vs|All Files (*.*)|*.*|";
    sDefExt = "ts";
  }
  else
  {
    sFilter = "Gocad Mix Files (*.mx)|*.mx|All Files (*.*)|*.*|";
    sDefExt = "mx";
  }

	// Default file name is documentfile name
	CString sDefaultFileName = GetGeomecDoc()->GetTitle();
  sDefaultFileName = RemoveExtension((const char *)sDefaultFileName, true).toStdString().c_str();

	CTnoFileDialog dlg(FALSE, sDefExt, sDefaultFileName, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, sFilter);
	if(dlg.DoModal() == IDOK)
	{
    CString sPath = dlg.GetPathName();
    QString pathName, fileName;

    SplitPathAndFileName((LPCSTR) sPath, pathName, fileName);
    fileName = RemoveExtension(fileName);

    UpdateData(TRUE);

    if(!(m_source == *m_pCopy))
      m_source = *m_pCopy;

    m_source.Export((LPCSTR) sPath, fileName);
  }
}

void CExportGocadDlg::OnAnalysisTypeChanged()
{
  UpdateData(TRUE);
  m_tcResults.UpdateTree();
}

void CExportGocadDlg::UpdateTimeStep()
{
	m_lbTimeStep.DeleteAllItems();

	// Insert depletion stages
	m_lbTimeStep.InsertColumn(0, "Depletion stage", LVCFMT_LEFT, 230);

	const CDepletionStage* pStage = &m_source.Model().InitialDepletionStage();
	while(pStage)
	{
		// Insert in list control
		new CDepletionStageObserver <CExportGocadDlg> (*pStage, *this, m_lbTimeStep);

		if(pStage->Last())
			pStage = 0;
		else
			pStage = &pStage->Next();
	}
}

BOOL CExportGocadDlg::Valid(const IValueComposite& composite) const
{

	assert(composite.ModeSize() > 0);
	for(int i = 0; i < composite.ComponentSize(); i++)
	{
		if(Valid(composite.Component(i)))
			return TRUE;
	}
	
	return FALSE;

}

static bool CheckResult(const CDepletionStage& stage, const CAnalysisType& antype, const IResultComponent& component)
{
	const IResult& result = dynamic_cast<const IResult&>(component.Parent());
	const IResultComponent* pComponent = result.ResultComponent( stage, antype, component.RegisterIndex(), component.ComponentIndex());
	if(pComponent) {
		if(pComponent->Defined()) return TRUE;
	}
	return FALSE;
}

BOOL CExportGocadDlg::Valid(const IValueComponentBase& component) const
{
	if(m_pCopy && (m_pCopy->HasFormations() || m_pCopy->HasFaults()))
  {
    bool bCanMapFormations = (m_pCopy->HasFormations() && component.CanMap(m_pCopy->FirstFormation()));
    bool bCanMapHorizons = (m_pCopy->HasFaults() && component.CanMap(m_pCopy->FirstFault()));
    if(!bCanMapFormations && !bCanMapHorizons)
      return FALSE;
/*
		if(m_pCopy->HasFormations())
    {
			if(!(component.CanMap(m_pCopy->FirstFormation())))
				return FALSE;
		}
*/
	}

	// Walk over modes
	const IResultComponent* pComponent = dynamic_cast<const IResultComponent*>(&component);
	if(pComponent)
  {
		int nComponent = component.ComponentIndex();	
		const IResult* pResult = dynamic_cast<const IResult*>(&pComponent->Parent());

    const CDepletionStage* pStage = &m_source.Model().InitialDepletionStage();
    while(pStage)
    {
      if(m_pCopy->DepletionStage(*pStage))
      {
        if(m_pCopy->Linear() && CheckResult(*pStage, CAnalysisType::AT_LINEAR, *pComponent))
          return TRUE;
        if(m_pCopy->NonLinear() && CheckResult(*pStage, CAnalysisType::AT_NONLIN, *pComponent))
          return TRUE;
        if(m_pCopy->Heat() && CheckResult(*pStage, CAnalysisType::AT_HEAT, *pComponent))
          return TRUE;
        if(m_pCopy->Mixture() && CheckResult(*pStage, CAnalysisType::AT_MIXTURE, *pComponent))
          return TRUE;
        if(m_pCopy->MixtureContainment() && CheckResult(*pStage, CAnalysisType::AT_MIXTURE_CONTAINMENT, *pComponent))
          return TRUE;
      }

      if(pStage->Last())
        pStage = 0;
      else
        pStage = &pStage->Next();
		}
	}
  else
  {
		return component.Defined();
	}

	return FALSE;
}

CModelBase& CExportGocadDlg::Model()
{
  return m_source.Model();
}

const CModelBase& CExportGocadDlg::Model() const
{
  return m_source.Model();
}

bool CExportGocadDlg::TimeStep(const CDepletionStage& stage) const
{
  return m_pCopy->DepletionStage(stage);
}

void CExportGocadDlg::TimeStep(const CDepletionStage& stage, bool bEnable)
{
	if(bEnable)
    m_pCopy->AddDepletionStage(stage);
	else
    m_pCopy->RemoveDepletionStage(stage);

	m_tcResults.UpdateTree();
}

bool CExportGocadDlg::Formation(const CFormationBase& formation) const
{
  return m_pCopy->Formation(formation);
}

void CExportGocadDlg::Formation(const CFormationBase& formation, bool bEnable)
{
  if(bEnable)
    m_pCopy->AddFormation(formation);
  else
    m_pCopy->RemoveFormation(formation);

  m_tcResults.UpdateTree();
}

bool CExportGocadDlg::Fault(const CTetraSubHorizon& fault) const
{
  return m_pCopy->Horizon(fault);
}

void CExportGocadDlg::Fault(const CTetraSubHorizon& fault, bool bEnable)
{
  if(bEnable)
    m_pCopy->AddHorizon(fault);
  else
    m_pCopy->RemoveHorizon(fault);

  m_tcResults.UpdateTree();
}

bool CExportGocadDlg::PointSet(const CPointSet& ptSet) const
{
  return m_pCopy->PointSet(ptSet);
}

void CExportGocadDlg::PointSet(const CPointSet& ptSet, bool bEnable)
{
  if(bEnable)
    m_pCopy->AddPointSet(ptSet);
  else
  m_pCopy->RemovePointSet(ptSet);

  m_tcResults.UpdateTree();
}

bool CExportGocadDlg::ResultComposite(const IValueComposite& composite) const
{
  return m_RGObserver->ResultComposite(composite);
}

void CExportGocadDlg::ResultComposite(const IValueComposite& composite, bool bEnable)
{
  m_RGObserver->ResultComposite(composite, bEnable);
}
