// ExportSvsDlg.cpp : implementation file
//
#include "StdAfx.h"
#include "Geomec.h"

#include "stdafx.h"
#include "Geomec.h"
#include "ExportSvsDlg.h"
#include "ModelBase.h"
#include "ResultRegister.h"
#include "TNOFileDialog.h"
#include "Result.h"
#include "ResultTree.h"
//#include "ExportCategories.h"
#include "ExportResultTreeBranch.h"
#include "ResultExporter.h"
#include "DepletionStageObserver.h"
#include "GeomecUtils.h"
#include "GlobalMessage.h"

// CExportSvsDlg dialog

IMPLEMENT_DYNAMIC(CExportSvsDlg, CDialog)

CExportSvsDlg::CExportSvsDlg(CModelBase &model, CWnd* pParent /*=NULL*/)
	: IExportDlg(CExportSvsDlg::IDD, pParent), m_Model(model)
  , m_bFormationNames(FALSE)
  , m_pCurrentSelection(0)
{
  m_bLinear = m_Model.ResultRegister().ResultsAvailable(CAnalysisType::AT_LINEAR);
  m_bNonLinear = m_Model.ResultRegister().ResultsAvailable(CAnalysisType::AT_NONLIN);
  m_bMixture = m_Model.ResultRegister().ResultsAvailable(CAnalysisType::AT_MIXTURE);
  m_bMixtureContainment = m_Model.ResultRegister().ResultsAvailable(CAnalysisType::AT_MIXTURE_CONTAINMENT);

  const CDepletionStage* pStage = &model.InitialDepletionStage();
	while(pStage)
	{
		VERIFY(m_stDeplStages.insert(pStage).second);

		if(pStage->Last())
			pStage = 0;
		else
			pStage = &pStage->Next();
	}
}

CExportSvsDlg::~CExportSvsDlg()
{
//  for(size_t i = 0; i < m_vcCategory.size(); i++) {
//		delete m_vcCategory[i];
//	}
}

void CExportSvsDlg::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CExportSvsDlg)
  DDX_Control(pDX, IDC_SVS_TC_CATEGORIES, m_tcCategories);
  DDX_Control(pDX, IDC_SVS_TC_DEPLST, m_lcDeplStages);
  DDX_Check(pDX, IDC_CHECK_SVS_LIN, m_bLinear);
  DDX_Check(pDX, IDC_CHECK_SVS_NLIN, m_bNonLinear);
  DDX_Check(pDX, IDC_CHECK_SVS_MIXT, m_bMixture);
  DDX_Check(pDX, IDC_CHECK_SVS_MIXTC, m_bMixtureContainment);
  DDX_Check(pDX, IDC_CHECK_SVS_FORM, m_bFormationNames);
  //}}AFX_DATA_MAP
}

BOOL CExportSvsDlg::OnInitDialog()
{
  CDialog::OnInitDialog();
  UpdateTimeSteps();

  GetDlgItem(IDC_CHECK_SVS_LIN)->EnableWindow(m_Model.ResultRegister().ResultsAvailable(CAnalysisType::AT_LINEAR));
  GetDlgItem(IDC_CHECK_SVS_NLIN)->EnableWindow(m_Model.ResultRegister().ResultsAvailable(CAnalysisType::AT_NONLIN));
  GetDlgItem(IDC_CHECK_SVS_MIXT)->EnableWindow(m_Model.ResultRegister().ResultsAvailable(CAnalysisType::AT_MIXTURE));
  GetDlgItem(IDC_CHECK_SVS_MIXTC)->EnableWindow(m_Model.ResultRegister().ResultsAvailable(CAnalysisType::AT_MIXTURE_CONTAINMENT));

  m_pFormationSelection = new TFormationSelection(Selection());
  re::CreateExportResultTreeBranch<re::CFormationBaseEntryObserver, TFormationBaseEntry, MD_BASE_FORMATION, TFormationSelection>(m_Model, *m_pFormationSelection, m_tcCategories);
  m_vcSelection.push_back(m_pFormationSelection);

  m_pPointSetSelection = new TPointSetSelection(Selection());
  re::CreateExportResultTreeBranch<re::CPointSetEntryObserver, TPointSetEntry, MD_BASE_POINTSET, TPointSetSelection>(m_Model, *m_pPointSetSelection, m_tcCategories);
  m_vcSelection.push_back(m_pPointSetSelection);

//  m_vcCategory.push_back(new TFormationCategory(*this, m_tcCategories, m_Model));
//  m_vcCategory.push_back(new CPointSetCategory(*this, m_tcCategories, m_Model));

  // Reset tree control and populate object tree
//	m_tcCategories.DeleteAllItems();
//	for(size_t i = 0; i < m_vcCategory.size(); i++) {
//		m_vcCategory[i]->populateTree();
//	}

  return TRUE;
}

void CExportSvsDlg::UpdateTimeSteps()
{
	m_lcDeplStages.DeleteAllItems();

	// Insert depletion stages
	m_lcDeplStages.InsertColumn(0, "Depletion stage", LVCFMT_LEFT, 230);

	const CDepletionStage* pStage = &m_Model.InitialDepletionStage();
	while(pStage)
	{
		// Insert in list control
		new CDepletionStageObserver <CExportSvsDlg> (*pStage, *this, m_lcDeplStages);

		if(pStage->Last())
			pStage = 0;
		else
			pStage = &pStage->Next();
	}
}

bool CExportSvsDlg::TimeStep(const CDepletionStage& stage) const
{ 
  return m_stDeplStages.find(&stage) != m_stDeplStages.end();
}

void CExportSvsDlg::TimeStep(const CDepletionStage& stage, bool bEnable) 
{ 
  if(!bEnable) m_stDeplStages.erase( &stage );
	if(bEnable ) m_stDeplStages.insert( &stage );
}

BEGIN_MESSAGE_MAP(CExportSvsDlg, CDialog)
  ON_BN_CLICKED(ID_EXPORT, &CExportSvsDlg::OnExport)
END_MESSAGE_MAP()

void CExportSvsDlg::OnSelect(const COpenGLNode& node)
{
  for(size_t i = 0; i < m_vcSelection.size(); ++i)
  {
    if(m_vcSelection[i]->CanSelect(node) && m_pCurrentSelection != m_vcSelection[i])
    {
      // clear current selection and set to new selection
      if(m_pCurrentSelection)
        m_pCurrentSelection->clear();
      m_pCurrentSelection = m_vcSelection[i];
      break;
    }
  }
//  if( m_current != &catagory ) {
		// Change of catagory reset all!
//		m_current = &catagory;

//		for(size_t i = 0; i < m_vcCategory.size(); i++) {
//			if(m_vcCategory[i] != &catagory) m_vcCategory[i]->clearSelection();
//		}
//  }
} 

void CExportSvsDlg::OnExport()
{
  if(!m_pCurrentSelection)
  {
    AfxMessageBox("Nothing selected.");
    return;
  }

  UpdateData(TRUE);
  if(!m_bLinear && !m_bNonLinear && !m_bMixture && !m_bMixtureContainment)
	{
		_m()->msg(IDS_NO_ANALYSIS_TYPE_SELECTED);
		return;
	}

	if(!m_stDeplStages.size())
	{
		_m()->msg(IDS_NO_TIMESTEP_SELECTED);
		return;
	}

  // Start file dialog
	// as soon as eclipse export is available, enable next line and delete line after that!!
	CString	sFilter = "ASCII Files (*.dat)|*.dat|Excel Files (*.xls;*.xls*)|*.xls;*.xls*|";
	// Add type for older format. Note that the index is used later to check the type!
	sFilter += "Old format ASCII Files (*.dat)|*.dat|Old format Excel Files (*.xls;*.xls*)|*.xls;*.xls*|";
	
	// Default file name is documentfile name
	CString sDefaultFileName = GetGeomecDoc()->GetTitle();
  sDefaultFileName = RemoveExtension((const char *)sDefaultFileName, true).toStdString().c_str();

	// Make some file dialog execute it and make it the path...
	CTnoFileDialog dlg(FALSE, "dat", sDefaultFileName, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, sFilter);

	if(dlg.DoModal() == IDOK)
	{
		// Get file node
		if(dlg.GetPathName().Right(4).CompareNoCase(".dat") == 0)
			OnExport(dlg.GetPathName(), false, dlg.m_ofn.nFilterIndex == 3);
		if(dlg.GetPathName().Right(4).CompareNoCase(".xls") == 0 || dlg.GetPathName().Right(5).Left(4).CompareNoCase(".xls") == 0)
			OnExport(dlg.GetPathName(), true, dlg.m_ofn.nFilterIndex == 4);
	}
}

bool CExportSvsDlg::OnExport(const CString& sFileName, bool bExcel, bool bOld)
{
  IExportFormat::CExportArg arg;
  arg.m_unit = CQuantity::SI_UNIT;
  arg.m_elementCenterPoints = IExportFormat::CENTER_ELEMENT;
  arg.m_bFormationNames = m_bFormationNames == 1 ? true : false;

  IExportFormat::TDataVec data;
  if(m_bLinear)
    FillResultVector(data, CAnalysisType::AT_LINEAR);
  if(m_bNonLinear)
    FillResultVector(data, CAnalysisType::AT_NONLIN);
  if(m_bMixture)
    FillResultVector(data, CAnalysisType::AT_MIXTURE);
  if(m_bMixtureContainment)
    FillResultVector(data, CAnalysisType::AT_MIXTURE_CONTAINMENT);


  arg.m_bOldFormat = bOld;

  CExporterFactory fac(m_Model);
  const IResultExporter* pExporter = 0;
  if(m_pCurrentSelection == m_pFormationSelection)
    pExporter = fac.CreateExporter(*m_pFormationSelection);
  else if(m_pCurrentSelection == m_pPointSetSelection)
    pExporter = fac.CreateExporter(*m_pPointSetSelection);

  assert(pExporter);

  if(pExporter)
  {
	  if( bExcel ) pExporter->ExportExcel((LPCSTR)sFileName, data, arg);
	  else         pExporter->ExportASCII((LPCSTR)sFileName, data, arg);
  }

  return true;
}

void CExportSvsDlg::FillResultVector(IExportFormat::TDataVec& vcRet, const CAnalysisType& antype)
{
	// Walk over timesteps
	const CDepletionStage* pStage = &m_Model.InitialDepletionStage();
	while(pStage)
	{
    if(m_stDeplStages.find(pStage) != m_stDeplStages.end())
    {
      vcRet.push_back(m_Model.ResultTree().TotalStress().MaxDirection()->ResultComponent(*pStage, antype, 0, 0));
      vcRet.push_back(m_Model.ResultTree().TotalStress().MaxDirection()->ResultComponent(*pStage, antype, 0, 1));
      vcRet.push_back(m_Model.ResultTree().TotalStress().MaxDirection()->ResultComponent(*pStage, antype, 0, 2));
      vcRet.push_back(m_Model.ResultTree().TotalStress().MaxDirection()->ResultComponent(*pStage, antype, 0, 3));
      vcRet.push_back(m_Model.ResultTree().TotalStress().MedDirection()->ResultComponent(*pStage, antype, 0, 0));
      vcRet.push_back(m_Model.ResultTree().TotalStress().MedDirection()->ResultComponent(*pStage, antype, 0, 1));
      vcRet.push_back(m_Model.ResultTree().TotalStress().MedDirection()->ResultComponent(*pStage, antype, 0, 2));
      vcRet.push_back(m_Model.ResultTree().TotalStress().MedDirection()->ResultComponent(*pStage, antype, 0, 3));
      vcRet.push_back(m_Model.ResultTree().TotalStress().MinDirection()->ResultComponent(*pStage, antype, 0, 0));
      vcRet.push_back(m_Model.ResultTree().TotalStress().MinDirection()->ResultComponent(*pStage, antype, 0, 1));
      vcRet.push_back(m_Model.ResultTree().TotalStress().MinDirection()->ResultComponent(*pStage, antype, 0, 2));
      vcRet.push_back(m_Model.ResultTree().TotalStress().MinDirection()->ResultComponent(*pStage, antype, 0, 3));
      vcRet.push_back(m_Model.ResultTree().PorePressure().ResultComponent(*pStage, antype));
      
    }

		if(pStage->Last())
			pStage = 0;
		else
			pStage = &pStage->Next();
	}
}
