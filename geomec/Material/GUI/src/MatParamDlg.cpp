#include "stdafx.h"
#include "geomec.h"

#include "MatParamDlg.h"

#include "TabModel.h"
#include "TabExperiment.h"
#include "LibraryMaterial.h"
#include "MaterialCreator.h"
#include "MaterialHelperFactory.h"
#include "RenameMaterialDlg.h"
#include "mlMaterialLibrary.h"
#include "GammaAxialView.h"
#include "GammaRadialView.h"
#include "ExperimentDataLimits.h"
#include "GeomecUtils.h"
#include "Diana.h"
#include "CalibSettingsDlg.h"
#include "DianaXWrapper.h"
#include "RunAnalysis.h"

#include "lbcx.h"
#include "lbfl.h"

#include "FilosFile.h" // dia::ff namespace

extern "C"
{
  void InitializeFilos();
}

#include "GlobalMessage.h"
#include "resourceIDI.h"
#include "Environment.h"
#include "DianaStartUp.h"
#include "TnoFileDialog.h"
#include "MaterialUnitTypes.h"
#include "ExcelAppGuard.h"
#include "resourceIDP.h"
#include "ExcelCell.h"
#include "ValueTypeFactory.h"
#include "PointSet.h"
#include "FormationBase.h"
#include "ModelBase.h"

//#include "SafeQueue.h"

//#include "Events.h"
#include "Global.h"

#include "DianaExecuter.h"
#include "RunAnalysis_CLI.h"

BEGIN_MESSAGE_MAP(CMatParamDlg, CDialog)
	ON_WM_SIZE()
	ON_WM_WINDOWPOSCHANGING()
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB, OnSelchangeTab)
	ON_BN_CLICKED(IDC_SIGMAEPSILON, OnSigmaepsilon)
	ON_BN_CLICKED(IDC_PQ, OnPq)
	ON_BN_CLICKED(IDC_FIT_ELASTIC, OnFitElastic)
	ON_BN_CLICKED(IDC_FIT_PLASTIC, OnFitPlastic)
  ON_BN_CLICKED(IDC_SETTINGS, OnSettings)
	ON_BN_CLICKED(IDC_NEXT, OnNext)
	ON_BN_CLICKED(IDC_PREVIOUS, OnPrevious)
  ON_BN_CLICKED(IDC_SI, OnSI)
  ON_BN_CLICKED(IDC_FIELD, OnField)
  ON_BN_CLICKED(IDC_EXPORT, OnExport)
END_MESSAGE_MAP()


CMatParamDlg::CMatParamDlg(CLibraryMaterial& mat, ml::CMaterialLibrary& matlib, int modelfilter, CAnalysisLogger& logger, CWnd* pParent)
: CDialogBase(IDD_PARAMETERSDLG, pParent),
  m_pModel(0),
  m_pOriginalMaterial(&mat),
  m_pMaterial(mat.Clone()),
  m_matlib(matlib),
  m_modelfilter(modelfilter),
  m_bInitialized(false),
  m_iGraphType(0),
  m_pViews(0),
  m_pResData(0),
  m_bFitElastic(true)
, m_matParamDlgDianaSignals(CDianaStartUp::instance(), this)
{
  CGeomecDoc *pDoc = GetGeomecDoc();
  m_nUnitDef = (pDoc->UnitNode().Unit() == IQuantityDouble::SI_UNIT ? 0 : 1);
  m_pModel = static_cast<CModelBase *>(pDoc->Model());
}

CMatParamDlg::~CMatParamDlg()
{
  delete m_pResData;
}

bool CMatParamDlg::checkAllowMaterialChange() // assume this is called after comparing old and new, and that they differ, so we don't need to test for that again
{
  if (m_pModel)
  {
    if (m_pMaterial->MaterialModel() != m_pOriginalMaterial->MaterialModel())
    {
    	TFormationBaseEntry *pEntry = dynamic_cast<TFormationBaseEntry *>(m_pModel->GraphEntry(MD_BASE_FORMATION));
	    assert(pEntry);
	    TFormationBaseEntry::TNodeSet stFormation = pEntry->EntryNodes();

	    for(TFormationBaseEntry::TNodeSet::iterator it = stFormation.begin(); it != stFormation.end(); ++it)
	    {
        bool bContains = false;
        int  nNrOfOtherMaterials = 0;

  	    CDepletionStage *pStage = &m_pModel->InitialDepletionStage();
	      while(pStage)
	      {
          if ((*it)->ConnectedMaterial(*pStage))
          {
            if ((*it)->ConnectedMaterial(*pStage)->LibraryMaterial())
            {
              if (&(*it)->ConnectedMaterial(*pStage)->LibraryMaterial()->LibraryMaterial() == m_pOriginalMaterial)
                bContains = true;
              else
                ++nNrOfOtherMaterials;
            }
          }

    	  	if (pStage->Last())
		      	pStage = 0;
		      else
			      pStage  = &pStage->Next();
	      }

        if (bContains && nNrOfOtherMaterials > 0)
          return false;
      }
    }
  }

  return true;
}

namespace
{

bool materialIsValid(const std::vector <QString>& materialParameterError)
{
  for (std::vector <QString>::const_iterator parameterError =
    materialParameterError.begin();
    parameterError != materialParameterError.end(); ++parameterError)
  {
    if (!(*parameterError).isEmpty())
    {
      return false;
    }
  }

  return true;
}

const QString WARN_4_INVALID_MATERIAL_PARAMETERS =
  QObject::tr("Warning: some material parameters are still not valid, "
    "the material will not be saved");

} // anonymous namespace

void CMatParamDlg::OnSave()
{
  if(*m_pMaterial != *m_pOriginalMaterial)
  {
    if (checkAllowMaterialChange())
    {
      if (materialIsValid(m_History.Current().materialParameterError()))
      {
        *m_pOriginalMaterial = *m_pMaterial;
      }
      else
      {
        _m()->msg(WARN_4_INVALID_MATERIAL_PARAMETERS, MB_OK);
      }
    }
    else
    {
      _m()->msg("Cannot change the material model in branches with different materials.");
    }
  }
}

void CMatParamDlg::OnSaveAs()
{
  if (!materialIsValid(m_History.Current().materialParameterError()))
  {
    _m()->msg(WARN_4_INVALID_MATERIAL_PARAMETERS, MB_OK);
  }
  else
  {
    CRenameMaterialDlg dlg(m_pMaterial->Name(), (mlMatModel)m_pMaterial->MaterialModel());
    if(dlg.DoModal() == IDOK)
    {
      m_pMaterial->Name(dlg.Name());
      m_matlib.AddMaterial(*m_pMaterial);
      m_pOriginalMaterial = m_pMaterial;
      m_pMaterial = m_pOriginalMaterial->Clone();
      UpdateCaption();
    }
  }
}

void CMatParamDlg::OnReset()
{
  *m_pMaterial = *m_pOriginalMaterial;
}

void CMatParamDlg::OnUpdateGraphs()
{
  CTabExperiment* pTab = (CTabExperiment*)m_pTab[1];
	pTab->OnFileListUpdated();
}

const CLibraryMaterial* CMatParamDlg::Material() const
{
  return m_pMaterial;
}

CLibraryMaterial* CMatParamDlg::Material()
{
  return m_pMaterial;
}

const CLibraryMaterial* CMatParamDlg::OriginalMaterial() const
{
  return m_pOriginalMaterial;
}

int CMatParamDlg::UnitDef() const
{
  return m_nUnitDef;
}

int CMatParamDlg::MaterialModelFilter() const
{
  return m_modelfilter;
}

void CMatParamDlg::InitializeModel(mlMatModel nModel)
{
  const CMaterialHelperFactory *f = CMaterialHelperFactory::Instance();

  ml::CMaterial::CCreator* pCreator = f->getMatCreator(nModel);
  if(pCreator)
  {
    m_pMaterial->SwitchMaterialModel(*pCreator, true);
  }
}

BOOL CMatParamDlg::OnInitDialog()
{
  CreateTabs();

  CDialog::OnInitDialog();

	HICON hIcon = AfxGetApp()->LoadIcon(IDI_MATLIB);
	SetIcon(hIcon, TRUE);
	SetIcon(hIcon, FALSE);

	CreateViews();
	CreateTabSheets();

	m_bInitialized = TRUE;
	CRect rectDlg;
	GetClientRect(&rectDlg);
	m_sizCurSize.cx = rectDlg.Width();
	m_sizCurSize.cy = rectDlg.Height();

//  CButton* pBack = (CButton*)GetDlgItem(IDC_PREVIOUS);
//  pBack->SetBitmap(::LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BACK)));

	GetDlgItem(IDC_FIT_ELASTIC)->EnableWindow(FALSE);
	GetDlgItem(IDC_FIT_PLASTIC)->EnableWindow(FALSE);
	GetDlgItem(IDC_PREVIOUS)->EnableWindow(FALSE);
	GetDlgItem(IDC_NEXT)->EnableWindow(FALSE);
	StoreControlPositions();

	GetDlgItem(IDC_FRA_GRAPH)->EnableWindow(FALSE);
	GetDlgItem(IDC_SIGMAEPSILON)->EnableWindow(FALSE);
	GetDlgItem(IDC_PQ)->EnableWindow(FALSE);

	m_iGraphType = 0;

	GetDlgItem(IDC_EXPORT)->EnableWindow(FALSE);

  // make sure graphs are drawn correctly
//  OnUpdateGraphsRequest(0, 0);

  UpdateCaption();

  CTabModel* tabModel = dynamic_cast <CTabModel*> (m_pTab[0]);

  AppendToHistory(tabModel->getMaterialParameterError());
  UpdateToCurrentHistoryItem();

	return TRUE;
}

void CMatParamDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMatParamDlg)
	DDX_Control(pDX, IDC_TAB, m_Tabs);
	//}}AFX_DATA_MAP
	DDX_Radio(pDX, IDC_SIGMAEPSILON, m_iGraphType);

	if(!pDX->m_bSaveAndValidate)
	{
		SetTabSheetData();
	}

	m_pTab[0]->UpdateData(pDX->m_bSaveAndValidate);
	m_pTab[1]->UpdateData(pDX->m_bSaveAndValidate);

  DDX_Radio(pDX, IDC_SI, m_nUnitDef);

	if(pDX->m_bSaveAndValidate)
	{
		GetTabSheetData();
	}
}

//
// not used by now but in the future some events could need to be blocked or forwarded
//
/*BOOL CMatParamDlg::PreTranslateMessage(MSG* pMsg)
{
	if( pMsg->message == ID_ON_EVENT )
		return TRUE;

  return CWnd::PreTranslateMessage(pMsg);
}*/

void CMatParamDlg::OnCancel()
{
  if (!m_pMaterial->validateMaterial())
  {
    return;
  }

  // modified?
  if(*m_pMaterial != *m_pOriginalMaterial)
  {
    int nID = _m()->msg("Do you want to save the modified material parameters?", MB_YESNOCANCEL);
    if(nID == IDCANCEL)
      return;
    if(nID == IDYES)
      OnSave();

    if ((nID == IDNO) && !m_pOriginalMaterial->validateMaterial())
    {
      return;
    }
  }

  CDialog::OnCancel();
}

void CMatParamDlg::CreateTabs()
{
	m_pTab[0] = new CTabModel(*this,&m_Tabs);
	m_pTab[1] = new CTabExperiment(*this,&m_Tabs);

	SetTabSheetData();

	m_pTab[0]->Create(((CTabModel *)m_pTab[0])->ID(), this);
	m_pTab[1]->Create(((CTabExperiment *)m_pTab[1])->ID(), this);
}

void CMatParamDlg::ResizeViews()
{
	CRect rectClient;
	GetClientRect(&rectClient);
	CWnd *pTabs = GetDlgItem(IDC_TAB);
	CRect rectTabs;
	pTabs->GetWindowRect(&rectTabs);

	rectClient.left += rectTabs.Width() + 20;
	{
          CRect rect;
          GetDlgItem(IDC_SETTINGS)->GetClientRect(&rect); // any button in this column
          rectClient.right -= rect.Width()+20;
	}
	rectClient.top += 10;
	rectClient.bottom -= 10;

	m_pViews->MoveWindow(&rectClient);
}

void CMatParamDlg::CreateViews()
{
	m_pViews = new CSplitterWnd;
	BOOL bRes = m_pViews->CreateStatic(this, 2, 1);

	CCreateContext Context;
	Context.m_pCurrentDoc = NULL;
	Context.m_pCurrentFrame = NULL;
	Context.m_pLastView = NULL;
	Context.m_pNewDocTemplate = NULL;
	Context.m_pNewViewClass = RUNTIME_CLASS(CGammaAxialView);

	CRect rectClient;
	GetClientRect(&rectClient);

	// 160 is just a placeholder value, 
	m_pViews->CreateView(0, 0, RUNTIME_CLASS(CGammaAxialView), CSize(160, rectClient.Height() / 2), &Context);

	Context.m_pNewViewClass = RUNTIME_CLASS(CGammaRadialView);
	m_pViews->CreateView(1, 0, RUNTIME_CLASS(CGammaRadialView), CSize(160, rectClient.Height() / 2), &Context);

	ResizeViews();
	m_pViews->ShowWindow(SW_SHOW);
	m_pViews->UpdateWindow();

	m_pGammaView[0] = (CGammaView *) m_pViews->GetPane(0, 0);
	m_pGammaView[1] = (CGammaView *) m_pViews->GetPane(1, 0);
}

void CMatParamDlg::CreateTabSheets()
{
	//Add tabs to control
	TCITEM pMyTab;
	pMyTab.mask=TCIF_TEXT;
  pMyTab.pszText=DiStrsave(QObject::tr("Model").toStdString().c_str());
	m_Tabs.InsertItem(0,&pMyTab);
  DiFree(pMyTab.pszText, "CMatParamDlg::CreateTabSheets");

	pMyTab.mask=TCIF_TEXT;
  pMyTab.pszText=DiStrsave(QObject::tr("Experiment").toStdString().c_str());
	m_Tabs.InsertItem(1,&pMyTab);
  DiFree(pMyTab.pszText, "CMatParamDlg::CreateTabSheets");

	CRect rectDlg;
	CRect rectTabs;

	m_pTab[0]->GetWindowRect(&rectDlg);
	m_Tabs.GetWindowRect(&rectTabs);
	rectDlg.OffsetRect(rectTabs.left - rectDlg.left + 1, rectTabs.top - rectDlg.top + 22);
	ScreenToClient(&rectDlg);

	m_pTab[1]->MoveWindow(&rectDlg, FALSE);
	m_pTab[1]->ShowWindow(SW_HIDE);

	m_pTab[0]->MoveWindow(&rectDlg, FALSE);
	m_pTab[0]->ShowWindow(SW_SHOW);
	m_pTab[0]->UpdateWindow();
}

void CMatParamDlg::StoreControlPositions()
{
	CRect rect;

	GetDlgItem(IDCANCEL)->GetWindowRect(&rect);
	m_ptClose.x = rect.left;
	m_ptClose.y = rect.top;
	ScreenToClient(&m_ptClose);

	GetDlgItem(ID_HELP)->GetWindowRect(&rect);
	m_ptHelp.x = rect.left;
	m_ptHelp.y = rect.top;
	ScreenToClient(&m_ptHelp);

	GetDlgItem(IDC_SETTINGS)->GetWindowRect(&rect);
	m_ptSettings.x = rect.left;
	m_ptSettings.y = rect.top;
	ScreenToClient(&m_ptSettings);

	GetDlgItem(IDC_FIT_ELASTIC)->GetWindowRect(&rect);
	m_ptFitElastic.x = rect.left;
	m_ptFitElastic.y = rect.top;
	ScreenToClient(&m_ptFitElastic);

	GetDlgItem(IDC_FIT_PLASTIC)->GetWindowRect(&rect);
	m_ptFitPlastic.x = rect.left;
	m_ptFitPlastic.y = rect.top;
	ScreenToClient(&m_ptFitPlastic);

	GetDlgItem(IDC_FRA_GRAPH)->GetWindowRect(&rect);
	m_ptGraphFrame.x = rect.left;
	m_ptGraphFrame.y = rect.top;
	ScreenToClient(&m_ptGraphFrame);

	GetDlgItem(IDC_SIGMAEPSILON)->GetWindowRect(&rect);
	m_ptSigEps.x = rect.left;
	m_ptSigEps.y = rect.top;
	ScreenToClient(&m_ptSigEps);

	GetDlgItem(IDC_PQ)->GetWindowRect(&rect);
	m_ptPQ.x = rect.left;
	m_ptPQ.y = rect.top;
	ScreenToClient(&m_ptPQ);

	GetDlgItem(IDC_FRA_UNITS)->GetWindowRect(&rect);
	m_ptUnitsFrame.x = rect.left;
	m_ptUnitsFrame.y = rect.top;
	ScreenToClient(&m_ptUnitsFrame);

	GetDlgItem(IDC_SI)->GetWindowRect(&rect);
	m_ptSI.x = rect.left;
	m_ptSI.y = rect.top;
	ScreenToClient(&m_ptSI);

	GetDlgItem(IDC_FIELD)->GetWindowRect(&rect);
	m_ptField.x = rect.left;
	m_ptField.y = rect.top;
	ScreenToClient(&m_ptField);

	GetDlgItem(IDC_EXPORT)->GetWindowRect(&rect);
	m_ptExport.x = rect.left;
	m_ptExport.y = rect.top;
	ScreenToClient(&m_ptExport);

	m_pViews->GetWindowRect(&rect);
	m_ptSplitterWnd.x = rect.left;
	m_ptSplitterWnd.y = rect.top;
	m_sizSplitterWnd.cx = rect.Width();
	m_sizSplitterWnd.cy = rect.Height();
	ScreenToClient(&m_ptSplitterWnd);
}

void CMatParamDlg::UpdateCaption()
{
  CString strCaption = _T("Material Parameters");
  if(m_pMaterial)
    strCaption = CString(m_pMaterial->Name().toStdString().c_str()) + " - " + strCaption;

  SetWindowText(strCaption);
}

void CMatParamDlg::SetTabSheetData()
{
/*
	CTabModel *pTab1 = (CTabModel *) m_pTab[0];

	assert(m_pMaterial != NULL);

	int iMaterialModel = m_pMaterial->MaterialModel();

	// set data in tab sheets
	pTab1->SelectedModel(iMaterialModel);
	pTab1->Type(m_iInputType);

  // combine globally defined locked parameters and material's locked parameters
  TNameSet stLockedParamNames;
  const TNameSet& stMatLockedParamNames = m_pMaterial->ReadOnlyParameters();

  std::set_union(m_stLockedParameterNames.begin(), m_stLockedParameterNames.end(),
                 stMatLockedParamNames.begin(), stMatLockedParamNames.end(),
                 std::insert_iterator<TNameSet>(stLockedParamNames, stLockedParamNames.end()));

  pTab1->SetLockedParameterNames(stLockedParamNames);
*/
}

void CMatParamDlg::GetTabSheetData()
{
/*
	CTabModel *pTab1 = (CTabModel *) m_pTab[0];

	// get data from tab sheets
	m_iMaterialModel = pTab1->SelectedModel();

	
	GetDlgItem(IDC_FIT_ELASTIC)->EnableWindow(MatIsCalib());
	GetDlgItem(IDC_FIT_PLASTIC)->EnableWindow(MatIsCalib());
		

	m_pMaterial->MaterialModel(m_iMaterialModel);
	m_iInputType = pTab1->Type();
*/
}

void CMatParamDlg::MoveControls()
{
	CRect rectDlg;
	GetClientRect(&rectDlg);
	CRect rectButton;

	GetDlgItem(IDCANCEL)->GetWindowRect(&rectButton);
	GetDlgItem(IDCANCEL)->MoveWindow
	( rectDlg.Width() - rectButton.Width()-15
	, m_ptClose.y, rectButton.Width(), rectButton.Height());

	GetDlgItem(ID_HELP)->GetWindowRect(&rectButton);
	GetDlgItem(ID_HELP)->MoveWindow
	( rectDlg.Width() - rectButton.Width()-15
	, m_ptHelp.y, rectButton.Width(), rectButton.Height());

	GetDlgItem(IDC_SETTINGS)->GetWindowRect(&rectButton);
	GetDlgItem(IDC_SETTINGS)->MoveWindow
	( rectDlg.Width() - rectButton.Width()-15
	, m_ptSettings.y, rectButton.Width(), rectButton.Height());

	GetDlgItem(IDC_FIT_ELASTIC)->GetWindowRect(&rectButton);
	GetDlgItem(IDC_FIT_ELASTIC)->MoveWindow
	( rectDlg.Width() - rectButton.Width()-15
	, m_ptFitElastic.y, rectButton.Width(), rectButton.Height());

	GetDlgItem(IDC_FIT_PLASTIC)->GetWindowRect(&rectButton);
	GetDlgItem(IDC_FIT_PLASTIC)->MoveWindow
	( rectDlg.Width() - rectButton.Width()-15
	, m_ptFitPlastic.y, rectButton.Width(), rectButton.Height());

	// Units radio buttons
	//
	CRect frameRect;
	GetDlgItem(IDC_FRA_UNITS)->GetWindowRect(&frameRect);
	GetDlgItem(IDC_FRA_UNITS)->MoveWindow
	( rectDlg.Width() - frameRect.Width()-15
	, m_ptUnitsFrame.y, frameRect.Width(), frameRect.Height());

	GetDlgItem(IDC_SI)->GetWindowRect(&rectButton);
	GetDlgItem(IDC_SI)->MoveWindow
	( rectDlg.Width() - frameRect.Width()-15
	, m_ptSI.y, rectButton.Width(), rectButton.Height());

	GetDlgItem(IDC_FIELD)->GetWindowRect(&rectButton);
	GetDlgItem(IDC_FIELD)->MoveWindow
	( rectDlg.Width() - frameRect.Width()-15
	, m_ptField.y, rectButton.Width(), rectButton.Height());
        //
	//---

	// Graph radio buttons
	//
	GetDlgItem(IDC_FRA_GRAPH)->GetWindowRect(&frameRect);
	GetDlgItem(IDC_FRA_GRAPH)->MoveWindow
	( rectDlg.Width() - frameRect.Width()-15
	, m_ptGraphFrame.y, frameRect.Width(), frameRect.Height());

	GetDlgItem(IDC_SIGMAEPSILON)->GetWindowRect(&rectButton);
	GetDlgItem(IDC_SIGMAEPSILON)->MoveWindow
	( rectDlg.Width() - frameRect.Width()-15
	, m_ptSigEps.y, rectButton.Width(), rectButton.Height());

	GetDlgItem(IDC_PQ)->GetWindowRect(&rectButton);
	GetDlgItem(IDC_PQ)->MoveWindow
	( rectDlg.Width() - frameRect.Width()-15
	, m_ptPQ.y, rectButton.Width(), rectButton.Height());
        //
	//---

	GetDlgItem(IDC_EXPORT)->GetWindowRect(&rectButton);
	GetDlgItem(IDC_EXPORT)->MoveWindow
	( rectDlg.Width() - rectButton.Width()-15
	, m_ptExport.y, rectButton.Width(), rectButton.Height());

	ResizeViews();
	RedrawWindow();

        // Vertical Resize;
        const int vdiff= rectDlg.Height() - m_sizCurSize.cy ;
        if ( vdiff != 0 )
        {
          // http://support.microsoft.com/kb/143291
          CRect rect;
          m_Tabs.GetWindowRect(&rect);
          ScreenToClient(&rect);
          rect.bottom += vdiff;
          m_Tabs.MoveWindow(&rect);

          m_pTab[0]->GetWindowRect(&rect);
          ScreenToClient(&rect);
          rect.bottom += vdiff;
          m_pTab[0]->MoveWindow(&rect);

          ((CTabModel *)m_pTab[0])->VerticalResize( vdiff);
        }


	m_sizCurSize.cx = rectDlg.Width();
	m_sizCurSize.cy = rectDlg.Height();
}

void CMatParamDlg::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here
	if(m_bInitialized)
	{
		MoveControls();

		if(m_iGraphType == 0)
		{
			CRect rectView;
			m_pViews->GetClientRect(&rectView);
			m_pViews->SetRowInfo(0, rectView.Height() / 2, 0);
			m_pViews->RecalcLayout();
		}
	}
}

void CMatParamDlg::OnWindowPosChanging(WINDOWPOS *lpwndpos)
{
	CDialog::OnWindowPosChanging(lpwndpos);
}

void CMatParamDlg::OnSelchangeTab(NMHDR* pNMHDR, LRESULT* pResult) 
{
	CTabCtrl *pTabCtrl = (CTabCtrl *) GetDlgItem(IDC_TAB);

	int iTab = pTabCtrl->GetCurSel();

	// hide all tabs
	for(int i=0; i<NR_TABS; ++i)
	{
		m_pTab[i]->ShowWindow(SW_HIDE);
	}

	// show selected tab
	m_pTab[iTab]->ShowWindow(SW_SHOW);

	*pResult = 0;
}

void CMatParamDlg::OnSigmaepsilon() 
{
	m_iGraphType = 0;
  CTabExperiment* pTab = (CTabExperiment*)m_pTab[1];
	pTab->OnFileListUpdated();
}

void CMatParamDlg::OnPq() 
{
	m_iGraphType = 1;
  CTabExperiment* pTab = (CTabExperiment*)m_pTab[1];
	pTab->OnFileListUpdated();
}

void CMatParamDlg::OnFitElastic()
{
  m_bFitElastic = true;
  CTabExperiment *pTab = (CTabExperiment *) m_pTab[1];

  for(int i = 0; i < pTab->ExperimentData().size(); ++i)
  {
    pTab->ExperimentData()[i].SetFitType(CExperimentData::ELASTIC);
  }

  CExperimentDataLimits dlg(pTab->ExperimentData(), pTab);
  if (dlg.DoModal() == IDOK)
  {
    // wjrx mantis 3235
    //
    // Create a temporary linear material (MM_LINEAR)
    // from the existing material.
    // Copy the applicable parameters (TRUE).
    //
    // This new material is used for calibration, we only need
    // Young's Modulus en Poisson Ratio from it.
    //
    CLibraryMaterial* pLinearMat = Material()->Clone();

    const CMaterialHelperFactory *f = CMaterialHelperFactory::Instance();

    ml::CMaterial::CCreator* pCreator = f->getMatCreator(MM_LINEAR);
    pLinearMat->SwitchMaterialModel(*pCreator, true);

    // Keep the pointer to the original material
    CLibraryMaterial* pOriginalMat = Material();

    // Calibrate the new linear material
    m_pMaterial = pLinearMat;
    bool bRet = Calibrate();

    if(bRet)
    {
      // Get Young's Modulus en Poisson Ratio from the calibrated
      // linear material and assign them to the original material.
      if(pOriginalMat->Parameter(MLD_YOUNGMODULUS))
        pOriginalMat->Parameter(MLD_YOUNGMODULUS)->Value(pLinearMat->Parameter(MLD_YOUNGMODULUS)->Value());
      if(pOriginalMat->Parameter(MLD_POISSONRATIO))
        pOriginalMat->Parameter(MLD_POISSONRATIO)->Value(pLinearMat->Parameter(MLD_POISSONRATIO)->Value());
    }

    // Set the material in the dialog back to the original material
    m_pMaterial = pOriginalMat;

    if(bRet)
      AppendToHistory();

    // Set the material parameter values and symbols in the list control
    CTabModel *pTM = (CTabModel*)(m_pTab[0]);
    pTM->UpdateControls();

    // update graphs
    CTabExperiment* pTE = (CTabExperiment*)(m_pTab[1]);
    pTE->OnFileListUpdated();

    Invalidate();
    UpdateWindow();
    
    pCreator->Destroy(pLinearMat);
  }
}

void CMatParamDlg::OnFitPlastic() 
{
  m_bFitElastic = false;
  CTabExperiment* pTab = (CTabExperiment *) m_pTab[1];

  for(int i = 0; i < pTab->ExperimentData().size(); ++i)
  {
    pTab->ExperimentData()[i].SetFitType(CExperimentData::PLASTIC);
  }

  CExperimentDataLimits dlg(pTab->ExperimentData(), pTab);
  if(dlg.DoModal() == IDOK)
  {
    // remember values for Young and Poisson
    geo::CValue valYoung;
    bool bYoungFixed;
    geo::CValue valPoisson;
    bool bPoissonFixed;
    if(m_pMaterial->Parameter(MLD_YOUNGMODULUS))
    {
      valYoung = m_pMaterial->Parameter(MLD_YOUNGMODULUS)->Value();
      bYoungFixed = m_pMaterial->Parameter(MLD_YOUNGMODULUS)->IsCurrentlyFixed();
      m_pMaterial->Parameter(MLD_YOUNGMODULUS)->CurrentlyFixed(true);
    }

    if(m_pMaterial->Parameter(MLD_POISSONRATIO))
    {
      valPoisson = m_pMaterial->Parameter(MLD_POISSONRATIO)->Value();
      bPoissonFixed = m_pMaterial->Parameter(MLD_POISSONRATIO)->IsCurrentlyFixed();
      m_pMaterial->Parameter(MLD_POISSONRATIO)->CurrentlyFixed(true);
    }

    bool bRet = Calibrate();

    if(valYoung.Valid())
    {
      m_pMaterial->Parameter(MLD_YOUNGMODULUS)->Value(valYoung.Value());
      m_pMaterial->Parameter(MLD_YOUNGMODULUS)->CurrentlyFixed(bYoungFixed);
    }

    if(valPoisson.Value())
    {
      m_pMaterial->Parameter(MLD_POISSONRATIO)->Value(valPoisson.Value());
      m_pMaterial->Parameter(MLD_POISSONRATIO)->CurrentlyFixed(bPoissonFixed);
    }

    if(bRet)
      AppendToHistory();

    // Set the material parameter values and symbols in the list control
    CTabModel *pTM = (CTabModel*)(m_pTab[0]);
    pTM->UpdateControls();

    // update graphs
    CTabExperiment* pTE = (CTabExperiment*)(m_pTab[1]);
    pTE->OnFileListUpdated();

    Invalidate();
    UpdateWindow();
    
  }
}

void CMatParamDlg::OnSettings()
{
  CCalibSettingsDlg dlg(m_pMaterial->ConvCriterion(), m_pMaterial->MaxIterations(), this);
  if(dlg.DoModal() == IDOK)
  {
    m_pMaterial->ConvCriterion(dlg.ConvergenceCriterion());
    m_pMaterial->MaxIterations(dlg.MaxNumIterations());
  }
}

void CMatParamDlg::AppendToHistory(
  const std::vector <QString>& materialParameterError)
{
  m_History.Append(
    CHistoryItem(*m_pMaterial, m_pResData, materialParameterError));
  UpdateHistoryButtons();
}

void CMatParamDlg::adjustCalculatedData(int nSelected, const int* piSelected)
{
  if (m_pResData != 0)
  {
    m_pResData->adjustCalculatedData(nSelected, piSelected);
  }
}

void CMatParamDlg::UpdateToCurrentHistoryItem()
{
  const CHistoryItem& item = m_History.Current();
  *m_pMaterial = item.Material();
  delete m_pResData;
  m_pResData = (item.ResultData() ? new CResultData(*item.ResultData()) : 0);

  // Set the material parameter values and symbols in the list control
  CTabModel *pTM = (CTabModel*)(m_pTab[0]);
  pTM->setMaterialParameterError(item.materialParameterError());
  pTM->UpdateControls();

  // update graphs
  CTabExperiment* pTE = (CTabExperiment*)(m_pTab[1]);
  pTE->OnFileListUpdated();
}

void CMatParamDlg::UpdateHistoryButtons()
{
  GetDlgItem(IDC_NEXT)->EnableWindow(m_History.CanGoForward());
  GetDlgItem(IDC_PREVIOUS)->EnableWindow(m_History.CanGoBack());
}

void CMatParamDlg::OnNext()
{
  m_History.GoForward();
  UpdateToCurrentHistoryItem();
  UpdateHistoryButtons();
}

void CMatParamDlg::OnPrevious()
{
  m_History.GoBack();
  UpdateToCurrentHistoryItem();
  UpdateHistoryButtons();
}

void CMatParamDlg::OnSI()
{
  m_nUnitDef = 0;
  UpdateToCurrentHistoryItem();
}

void CMatParamDlg::OnField()
{
  m_nUnitDef = 1;
  UpdateToCurrentHistoryItem();
}

void CMatParamDlg::OnExport()
{
  CString sDefExt = ".xlsx";
  CString sDefaultFileName = GetGeomecDoc()->GetTitle();

  sDefaultFileName =
    RemoveExtension((LPCTSTR) sDefaultFileName).toStdString().c_str();

  CString sFilter = "Excel Workbook (*.xlsx)|*.xlsx|";
  CTnoFileDialog dlg(FALSE, sDefExt, sDefaultFileName,
    OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, sFilter);

  if (dlg.DoModal() == IDOK)
  {
    exportCalibratedData((LPCTSTR) dlg.GetPathName());
  }
}


//
// here workflow is simpler:
//
//	- regular: ra-drc-runner-dxw-dlg-dsu
//	- here: dxw-dlg-dsu
//
bool CMatParamDlg::Calibrate() 
{
  QString strTempPath = GetGeomecTempPathExt(CTempPath::TEMP_GENERAL);
	if(strTempPath.right(1) != "/" && strTempPath.right(1) != "\\")
		strTempPath += "\\";

	char* filos_path = vDiStrsave(strTempPath.toStdString().c_str(), "Calib.ff", 0); 
	//QString strBaseName = "Calib";
	EnableWindow(FALSE);

	CDianaStartUp* dsu = CDianaStartUp::instance();

	QString DIAPATH_ = dsu->GetDianaEnv( CEnvironment::DIAPATH );

	//
	// STP0: special case as default one is 'gm42.exe'
	//

	//env
	dsu->SetDianaEnv( CEnvironment::STP0, DIAPATH_ + "/binseg/ap/" + "mc41.exe" ); // no standard
	dsu->SetDianaEnv( CDianaStartUp::FFDIR, strTempPath );
	dsu->SetDianaEnv( CDianaStartUp::FF, "Calib.ff");
	// vars
	dsu->var( CDianaStartUp::BASE, "Calib");
	//CDianaStartUp::DISPLAY

	//
	// create FF
	//
	WriteToFilos(filos_path);

	IExecuteDianaDialog* dlg = nullptr;


	IDianaXWrapper* dxw = nullptr;

	//
	// mcr 2020-07-28
	//
	// FIXME: 
	//
	//	dsa: 
	//		first execution: done but 'Out of memory' popup
	//		second execution: hangs
	//
	//	non-dsa:
	//		second execution: hangs ... it seems this was already there before last updates
	//
	//bool dsa_ = _g->dsa();
	//_g->dsa( false );

  bool res=true; // error
  
  if( _g->dsa() )
  {
	  //
	  // disable diana msg cmds sent to gui thread (doc)
	  //
	  //_e->disable(CEvents::GuiApp_EH);
	  //

	  dxw = new CDianaExecuter();
	  CRunAnalysis_CLI ra(dxw);
	  ra.wait();
	  res = ra.res() ? 0:1;	// in the thread logic success is true and fail is false
							// in this function logic is exactly the opposite
    
    //
	  // too soon: some diana msg cmds still queued so crash 
	  //
	  //_e->enable(CEvents::GuiApp_EH);

	  //
	  // window disabled during diana run
	  //
	  EnableWindow( TRUE );
	  //
  }
  else
  {
	  //
	  // TODO: in non-dsa this dlg enables the window (disabled while running diana)
	  //
	  dlg = CRunAnalysis::create_dlg(); // uses _g->dsa()
	  dxw = new CDianaXWrapper(dlg);
	  res = dxw->ExecuteDiana();
	  //res = diaexec.ExecuteDiana();
  }

  //
  // restore the saved value 
  //
  //_g->dsa( dsa_ );
  //
  //
  //


  if( res != 0 )
  // MCR: FF looked not Ok, should be only the file and not the full file path
  //if(diaexec.ExecuteDiana("mc41.exe", strTempPath, "", "", filos_path, strBaseName) != 0)
  {
		AfxMessageBox("Can not start DIANA calibration kernel.");
		return false;
  }

  //
  // special treatment wnhen dxw is a 'CDianaExecuter' and 'STP0' is 'mc41'
  //
  if(!dxw->GetCalculationResult())
  {
	  //
	  // this always happens int gui thread
	  //
		AfxMessageBox("Calculation failed.");
		if(m_pResData != NULL) 
			delete m_pResData;
		m_pResData = NULL;
		return false;
  }

	if(m_pResData != NULL) 
		delete m_pResData;
	m_pResData = new CResultData();

	// READ from FILOS *******************************************************************

	assert(filos_path);
	dia::ff::open(filos_path, O_RDWR);

	// Read the simulation results
	bool bResult = m_pResData->ReadFromFilos();

	// Read the new material parameters
	int bMatResult = m_pMaterial->ReadFromFilos();

	// close the filos file
	if(fcisop_() != 0)
		dia::ff::close();
	
	// ***********************************************************************************


	if(bMatResult < 1)
	{
		if (bMatResult == 0)
			AfxMessageBox("Unable to read calibrated material parameters.");
		else
			AfxMessageBox("Diana changed fixed parameters: rejecting the results.");
    delete m_pResData;
    m_pResData = 0;
    return false;
	}

	if(!bResult)
	{
		AfxMessageBox("Unable to read results from calibration.", MB_OK | MB_HELP);
		delete m_pResData;
		m_pResData = 0;
    return false;
	}

	_unlink(filos_path);
	DiFree(filos_path, "CMatParametersDlg::OnCalibrate");

	if( !_g->dsa() )
		CRunAnalysis::delete_dlg( dlg );
	delete dxw;

	//if(_g->dsa())
	//	_e->enable(CEvents::GuiApp_EH);

  return true;
}

void CMatParamDlg::WriteToFilos(const char* filos_path)
{
  assert(m_pMaterial != NULL);
  CTabExperiment* pTab = (CTabExperiment*) m_pTab[1];

  int i;
  CExperimentArray vcData = pTab->ExperimentData();

  assert(filos_path);
  ftn_int_t FilosDefs[20];
  gtfdef_(FilosDefs);
  InitFilosFile(filos_path, FilosDefs, 0);
  dia::ff::open(filos_path, O_RDWR);

  InitializeFilos();

  int j;
  ftn_double_t dValue;
  ftn_int_t nValue;
  ftn_int_t index;
  CExperimentData current_data;
  CStressStrain current_ss;

  PushDir();
  index = 10;
  ChangeDir("/CMNDS/");
  PushDir();
  ChangeIndexedDir("SEGINF/", &index);
  PutCharItem("MODULE", "AP");
  PutCharItem("SEGMEN", "MATCAL");
  PushDir();
  ChangeDir("MATCAL/");
  dValue = m_pMaterial->ConvCriterion();
  PutItem("NORM", &dValue);
  nValue = m_pMaterial->MaxIterations();
  PutItem("MITERA", &nValue);
  PopDir();
  PopDir();
  PushDir();
  index = 20;
  ChangeIndexedDir("SEGINF/", &index);
  PutCharItem("MODULE", "DC");
  PutCharItem("SEGMEN", "END");
  PopDir();
  PopDir();
  PushDir();	
  ChangeDir("/MATCAL/");
  for(i = 0; i < vcData.size(); i++)
  {
    PushDir();
    index = i + 1;
    current_data = vcData[i];

    // wjrx mantis 3158
    double lowerLimitFraction= current_data.getLowerLimitFraction();
    double upperLimitFraction= current_data.getUpperLimitFraction();

    int lowerLimitIndex=
      lowerLimitFraction * current_data.GetStressStrainSteps().size();

    ChangeIndexedDir("EXPERI/", &index); 
    dValue = current_data.WeightFactor();

    PutItem("WEIGHT", &dValue);
    for(j = lowerLimitIndex
       ; j < upperLimitFraction*current_data.GetStressStrainSteps().size()
       ; ++j)
    {
      PushDir();
      index = j + 1 - lowerLimitIndex;
      current_ss = current_data.GetStressStrainSteps()[j];
      ChangeIndexedDir("STEP/", &index);
      PushDir();
      ftn_double_t stress[2];
      ftn_double_t strain[2];
      
      stress[0] = -1.0 * current_ss.m_dAxialStress;
      stress[1] = -1.0 * current_ss.m_dRadialStress;
      PutItemLength("STRESS", stress, 2);

      ftn_double_t pressure = current_ss.m_dPressure;
      PutItem("PRESSU", &pressure);

      // wjrx mantis 3199
      strain[0] = -1.0 * (current_ss.m_dAxialStrain - current_data.getAxialStrainOffset());
      strain[1] = -1.0 * (current_ss.m_dRadialStrain - current_data.getRadialStrainOffset());
      PutItemLength("STRAIN", strain, 2);

      dValue = current_ss.m_dTime;
      PutItem("TIME", &dValue);
      PopDir();
      PopDir();
    }

    PopDir();
  }
  PopDir();

  int svfi = SetFunctionInfoFlags(1, DS_FUNC_WRITE);
  ftn_int_t idx = 10;
  PutItem("/CMNDS/JCLPOS", &idx);
  SetFunctionInfoFlags(svfi, DS_FUNC_WRITE);

  // wjrx mantis 3158
//  std::set<CString> stLockedParameterNames(m_stLockedParameterNames);
//  CString name;

  /*
   * In order to insert valid parameter names
   * we assume that getStringTableEntry(...) returns "" when
   * the entry does not exist, the string table will not contain
   * empty values!
   */
/*
  if (m_bFitElastic)
  {
    if ((name = getStringTableEntry(IDS_HARDENING).toStdString().c_str()) != "")
      stLockedParameterNames.insert(name);
    if ((name = getStringTableEntry(IDS_SEC_HARD).toStdString().c_str()) != "")
      stLockedParameterNames.insert(name);
    if ((name = getStringTableEntry(IDS_CAPSHAPE).toStdString().c_str()) != "")
      stLockedParameterNames.insert(name);
    if ((name = getStringTableEntry(IDS_PRECONSOLIDATION).toStdString().c_str()) != "")
      stLockedParameterNames.insert(name);
    if ((name = getStringTableEntry(IDS_SEC_PRECON).toStdString().c_str()) != "")
      stLockedParameterNames.insert(name);
    if ((name = getStringTableEntry(IDS_INITFRICTION).toStdString().c_str()) != "")
      stLockedParameterNames.insert(name);
    if ((name = getStringTableEntry(IDS_COHESION).toStdString().c_str()) != "")
      stLockedParameterNames.insert(name);
  } 
  else
  {
    if ((name = getStringTableEntry(IDS_YOUNGMODULUS).toStdString().c_str()) != "")
      stLockedParameterNames.insert(name);
    if ((name = getStringTableEntry(IDS_POISSONRATIO).toStdString().c_str()) != "")
      stLockedParameterNames.insert(name);
  }
*/
  if(!m_pMaterial->WriteToFilos())
  {
    AfxMessageBox("Unable to write calibration input to Filos file.");
    EnableWindow(TRUE);
    return;
  }

  // close the filos file
  if(fcisop_() != 0) 
	  dia::ff::close();

  // ******************************************************************
}

void CMatParamDlg::OnChildViewSized()
{
	if(m_iGraphType == 1) // PQ plots, don't allow moving the splitter
	{
		CRect rectView;
		m_pViews->GetClientRect(&rectView);
		m_pViews->SetRowInfo(0, rectView.Height(), 0);
		m_pViews->RecalcLayout();
	}
}

void CMatParamDlg::OnExperimentFileListUpdated(int nItems, int nSelected, const int* piSelected,
  bool useInitialLowerLimit, const std::vector <int>& initialLowerLimit)
{
	GetDlgItem(IDC_FIT_ELASTIC)->EnableWindow(nItems > 0 && m_pMaterial->CanCalibrateElastic());
	GetDlgItem(IDC_FIT_PLASTIC)->EnableWindow(nItems > 0 && m_pMaterial->CanCalibratePlastic());
	GetDlgItem(IDC_EXPORT)->EnableWindow(nItems > 0);

	UpdateGraphs(nItems, nSelected, piSelected,
		useInitialLowerLimit, initialLowerLimit);
}

void CMatParamDlg::UpdatePQRadios(bool bHasItems)
{
	GetDlgItem(IDC_FRA_GRAPH)->EnableWindow(bHasItems);
	GetDlgItem(IDC_SIGMAEPSILON)->EnableWindow(bHasItems);
	GetDlgItem(IDC_PQ)->EnableWindow(bHasItems);

	if(m_iGraphType == -1 && bHasItems)
		m_iGraphType = 0;

	if(!bHasItems)
		m_iGraphType = -1;

	UpdateData(FALSE);
}

void CMatParamDlg::UpdateGraphs(int nItems, int nSelected, const int *piSelected,
  bool useInitialLowerLimit, const std::vector <int>& initialLowerLimit)
{
	CTabExperiment *pTab = (CTabExperiment *) m_pTab[1];
	CExperimentArray aData;
	CResultData *pResData = 0;

  collectExperimentData(pTab, aData, &pResData, nItems, nSelected, piSelected);

	if(m_iGraphType == 1)
    UpdatePQGraph(aData, pResData, m_pMaterial);
	else
    UpdateSigmaEpsilonGraph(aData, pResData,
      useInitialLowerLimit, initialLowerLimit);

	if(pResData)
    delete pResData;
}

void CMatParamDlg::UpdateSigmaEpsilonGraph(const CExperimentArray &aData, const CResultData *pResData,
  bool useInitialLowerLimit, const std::vector <int>& initialLowerLimit)
{
	CRect rectView;
	m_pViews->GetClientRect(&rectView);
	m_pViews->SetRowInfo(0, rectView.Height() / 2, 0);
	m_pViews->RecalcLayout();

	m_pGammaView[0]->PlotData(aData, pResData, TRUE, TRUE, mlUnitDef(m_nUnitDef),
		useInitialLowerLimit, initialLowerLimit);
	m_pGammaView[1]->PlotData(aData, pResData, FALSE, FALSE, mlUnitDef(m_nUnitDef),
		useInitialLowerLimit, initialLowerLimit);
}

void CMatParamDlg::UpdatePQGraph(const CExperimentArray &aData, const CResultData *pResData, const CLibraryMaterial *pMat)
{
	if(!pMat)
    return;

	CRect rectView;
	m_pViews->GetClientRect(&rectView);
	m_pViews->SetRowInfo(0, rectView.Height(), 0);
	m_pViews->RecalcLayout();

	m_pGammaView[0]->PlotPQ(aData, pResData, TRUE, pMat, mlUnitDef(m_nUnitDef));
	m_pGammaView[1]->PlotPQ(aData, pResData, FALSE, pMat, mlUnitDef(m_nUnitDef));
}

void CMatParamDlg::collectExperimentData(CTabExperiment* pTab,
  CExperimentArray& aData, CResultData** pResData, int nItems, int nSelected,
  const int* piSelected) const
{
	bool bShowAll = pTab->ShowAllExperiments();

	if(m_pResData) *pResData = new CResultData(*m_pResData);

	if(bShowAll)
	{
		aData.resize(pTab->ExperimentData().size());
		for(int i=0; i<aData.size(); ++i)
		{
			pTab->ExperimentData()[i].Selected(true);
			aData[i] = pTab->ExperimentData()[i];
		}
	}
	else
	{
		for(int i=0; i< pTab->ExperimentData().size(); ++i)
		{
			pTab->ExperimentData()[i].Selected(false);
    }

		aData.resize(nSelected);
		for(int i=0; i<aData.size(); ++i)
		{
			pTab->ExperimentData()[piSelected[i]].Selected(true);
			aData[i] = pTab->ExperimentData()[piSelected[i]];
		}
		// make pResData a subset
		if(*pResData)
      (*pResData)->GoSubSet(piSelected, nSelected);
	}
}

void CMatParamDlg::OnDianaXCloseDialog()
{
	EnableWindow( TRUE );
}

void CMatParamDlg::OnDianaXDiaMessage(LPCTSTR strMessage)
{
//	m_Logger.AddLine(strMessage);
}

void CMatParamDlg::OnDianaXDiaWarning(LPCTSTR strMessage)
{
//	m_Logger.AddLine(strMessage);
}

void CMatParamDlg::OnDianaXDiaError(LPCTSTR strMessage)
{
//	m_Logger.AddLine(strMessage);
}

// private

void CMatParamDlg::exportCalibratedData(const QString& fileName) const
{
  CTabExperiment* pTab = dynamic_cast <CTabExperiment*> (m_pTab[1]);
  CExperimentArray aData;
  CResultData* pResData = 0;
  int nItems = 0;
  int nSelected = 0;
  int* piSelected = 0;

  pTab->getSelectedItems(nItems, nSelected, &piSelected);
  collectExperimentData(pTab, aData, &pResData, nItems, nSelected, piSelected);

  if (aData.size() > 0)
  {
    exportCalibratedData(fileName, aData, pResData);
  }

  delete [] piSelected;

  if (pResData)
  {
    delete pResData;
  }
}

void CMatParamDlg::exportCalibratedData(const QString& fileName,
  const CExperimentArray& experimentArray, const CResultData* resultData) const
{
  try
  {
    CExcelAppGuard ExcelApp;
    VARIANT vNotPassed;

    V_VT(&vNotPassed) = VT_ERROR;
    V_ERROR(&vNotPassed) = DISP_E_PARAMNOTFOUND;

    ExcelApp.excelApp().SetSheetsInNewWorkbook(experimentArray.size());

    Workbooks wbs;
    wbs.AttachDispatch(ExcelApp.excelApp().GetWorkbooks());

    _Workbook wb;

    wb.AttachDispatch(wbs.Add(vNotPassed));

    Worksheets wss;

    wss.AttachDispatch(wb.GetSheets());

    if (resultData)
    {
      assert(experimentArray.size() == resultData->GetNrExperiments());
    }

    for (short s = 1; s <= experimentArray.size(); ++s)
    {
      exportCalibratedData(wss, experimentArray, resultData, s);
    }

    wss.ReleaseDispatch();

    VARIANT vFileName, vFALSE;
    CString sFileName = fileName.toStdString().c_str();

    V_VT(&vFileName) = VT_BSTR;
    V_BSTR(&vFileName) = sFileName.AllocSysString();

    V_VT(&vFALSE) = VT_BOOL;
    V_BOOL(&vFALSE) = FALSE;

    BOOL displayAlerts = ExcelApp.excelApp().GetDisplayAlerts();

    ExcelApp.excelApp().SetDisplayAlerts(FALSE);
    wb.SaveAs(vFileName, vNotPassed, vNotPassed, vNotPassed, vNotPassed,
      vNotPassed, 0, vNotPassed, vNotPassed, vNotPassed, vNotPassed);
    ExcelApp.excelApp().SetDisplayAlerts(displayAlerts);
    wb.Close(vFALSE, vFileName, vNotPassed);
    SysFreeString(V_BSTR(&vFileName));
    wb.ReleaseDispatch();
    wbs.Close();
    wbs.ReleaseDispatch();
  }

  catch (CExcelAppGuard::CNoExcelException&)
  {
    _m()->msg(IDP_EXCELNOTINSTALLED, MB_OK | MB_HELP);
  }

  return;
}

void CMatParamDlg::exportCalibratedData(Worksheets& wss,
  const CExperimentArray& experimentArray, const CResultData* resultData,
  short s) const
{
  _Worksheet ws;

  ws.AttachDispatch(wss.GetItem(COleVariant(s)));

  CString sheetName = experimentArray[s - 1].GetLabel();

  sheetName = sheetName.Mid(sheetName.Find(',') + 1);
  ws.SetName(sheetName.Trim());

  exportCalibratedData(ws, experimentArray[s - 1], resultData, s);

  ws.ReleaseDispatch();
}

namespace
{

const double SECONDS_PER_MINUTE = 60.0;

typedef struct
{
  QString m_name;
  double m_value;
  QString m_unit;
} TMaterialParameter;

std::vector <TMaterialParameter> collectMaterialParameters(CFemAppModel& model,
  CLibraryMaterial* material, int unitDefinition)
{
  std::vector <TMaterialParameter> materialParameters;
  const CValueTypeFactory *valueTypeFactory = CValueTypeFactory::instance();
  CValueTypeFactory::TValueTypeVec valueTypes = valueTypeFactory->ValueTypes();
  CPointSet pointSet(model);
  mlUnitDef unitDef(unitDefinition == 0 ? MLUD_SI : MLUD_FIELD);

  for (size_t i = 0; i < material->ParameterSize(); ++i)
  {
    CLibraryMaterialParameter& libraryMaterialParameter =
      material->Parameter(i);
    int valueTypeID = libraryMaterialParameter.ValueTypeID();
    int nameIndex = valueTypeFactory->NameIndex(valueTypeID);

    if (nameIndex != -1)
    {
      CValueType* valueType =
        valueTypeFactory->BuildValueType(pointSet, valueTypeID, nameIndex);
      TMaterialParameter materialParameter;

      materialParameter.m_name = valueType->ExportType();
      materialParameter.m_value =
        libraryMaterialParameter.ValueToUserUnit(unitDef);
      materialParameter.m_unit = libraryMaterialParameter.UnitName(unitDef);

      materialParameters.push_back(materialParameter);
    }
  }

  return materialParameters;
}

} // anonymous namespace

void CMatParamDlg::exportCalibratedData(_Worksheet& ws,
  const CExperimentData& experimentData, const CResultData* resultData,
  short s) const
{
  CExcelCell cell(1, 1);
  const CStressStrainArray& strainSteps1 = experimentData.GetStressStrainSteps();
  size_t lowerSize =
    experimentData.getLowerLimitFraction() * strainSteps1.size();
  size_t upperSize =
    experimentData.getUpperLimitFraction() * strainSteps1.size();
  mlUnitDef unitDef(m_nUnitDef == 0 ? MLUD_SI : MLUD_FIELD);

  exportCalibratedDataHeader(cell, strainSteps1, resultData);
  exportCalibratedDataUnits(cell, strainSteps1, resultData);

  std::vector <TMaterialParameter> materialParameters =
    collectMaterialParameters(*GetGeomecDoc()->Model(), m_pMaterial,
      m_nUnitDef);
  size_t p = 0;

  for (; p < strainSteps1.size(); ++p)
  {
    cell.writeDouble(strainSteps1[p].m_dTime / SECONDS_PER_MINUTE);
    cell.nextColumn();
    cell.writeDouble(
      CUnitTypeStress().ToUserUnit(strainSteps1[p].m_dAxialStress, unitDef));
    cell.nextColumn();
    cell.writeDouble(
      CUnitTypeStress().ToUserUnit(strainSteps1[p].m_dRadialStress, unitDef));
    cell.nextColumn();
    cell.writeDouble(strainSteps1[p].m_dAxialStrain);
    cell.nextColumn();
    cell.writeDouble(strainSteps1[p].m_dRadialStrain);

    if (resultData && (lowerSize <= p) && (p <= upperSize))
    {
      const CStressStrainArray& strainSteps2 =
        resultData->GetStressStrainSteps()[s - 1];

      cell.nextColumn();
      cell.writeDouble(CUnitTypeStress().
        ToUserUnit(strainSteps2[p - lowerSize].m_dAxialStress, unitDef));
      cell.nextColumn();
      cell.writeDouble(CUnitTypeStress().
        ToUserUnit(strainSteps2[p - lowerSize].m_dRadialStress, unitDef));
    }
    else
    {
      cell.nextColumn();
      cell.nextColumn();
    }

    if (p < materialParameters.size())
    {
      cell.nextColumn();
      cell.WriteString(materialParameters[p].m_name.toStdString().c_str());
      cell.nextColumn();
      cell.writeDouble(materialParameters[p].m_value);
      cell.nextColumn();
      cell.WriteString(materialParameters[p].m_unit.toStdString().c_str());
    }

    cell.nextRow();
  }

  for (; p < materialParameters.size(); ++p)
  {
    cell.nextColumn();
    cell.nextColumn();
    cell.nextColumn();
    cell.nextColumn();
    cell.nextColumn();
    cell.nextColumn();
    cell.nextColumn();
    cell.WriteString(materialParameters[p].m_name.toStdString().c_str());
    cell.nextColumn();
    cell.writeDouble(materialParameters[p].m_value);
    cell.nextColumn();
    cell.WriteString(materialParameters[p].m_unit.toStdString().c_str());
    cell.nextRow();
  }

  cell.WriteToSheet(ws);
}

namespace
{

const CString CALCULATED = ", Calculated data";

} // anonymous namespace

void CMatParamDlg::exportCalibratedDataHeader(CExcelCell& cell,
  const CStressStrainArray& strainSteps, const CResultData* resultData) const
{
  if (strainSteps.size() > 0)
  {
    cell.WriteString(CExperimentData::getColumnHeader(CExperimentData::Time));
    cell.nextColumn();
    cell.WriteString(
      CExperimentData::getColumnHeader(CExperimentData::AxialStress));
    cell.nextColumn();
    cell.WriteString(
      CExperimentData::getColumnHeader(CExperimentData::RadialStress));
    cell.nextColumn();
    cell.WriteString(
      CExperimentData::getColumnHeader(CExperimentData::AxialStrain));
    cell.nextColumn();
    cell.WriteString(
      CExperimentData::getColumnHeader(CExperimentData::RadialStrain));

    if (resultData)
    {
      cell.nextColumn();
      cell.WriteString(
        CExperimentData::getColumnHeader(CExperimentData::AxialStress) +
          CALCULATED);
      cell.nextColumn();
      cell.WriteString(
        CExperimentData::getColumnHeader(CExperimentData::RadialStress) +
          CALCULATED);
    }

    cell.nextRow();
  }
}

namespace
{

const CString UNIT_TIME = "min";
const CString UNIT_STRAIN = "strain";

} // anonymous namespace

void CMatParamDlg::exportCalibratedDataUnits(CExcelCell& cell,
  const CStressStrainArray& strainSteps, const CResultData* resultData) const
{
  mlUnitDef unitDef(m_nUnitDef == 0 ? MLUD_SI : MLUD_FIELD);

  if (strainSteps.size() > 0)
  {
    cell.WriteString(UNIT_TIME);
    cell.nextColumn();
    cell.WriteString(CUnitTypeStress().UnitName(unitDef));
    cell.nextColumn();
    cell.WriteString(CUnitTypeStress().UnitName(unitDef));
    cell.nextColumn();
    cell.WriteString(UNIT_STRAIN);
    cell.nextColumn();
    cell.WriteString(UNIT_STRAIN);

    if (resultData)
    {
      cell.nextColumn();
      cell.WriteString(
        CUnitTypeStress().UnitName(unitDef));
      cell.nextColumn();
      cell.WriteString(
        CUnitTypeStress().UnitName(unitDef));
    }

    cell.nextRow();
  }
}

///// CMatParamDlg::CHistoryItem

CMatParamDlg::CHistoryItem::CHistoryItem(const CLibraryMaterial& mat,
  const CResultData* pRes, const std::vector <QString>& materialParameterError)
: m_pMat(mat.Clone()),
  m_pRes(pRes ? new CResultData(*pRes) : 0)
, m_materialParameterError(materialParameterError)
{
}

CMatParamDlg::CHistoryItem::CHistoryItem(const CHistoryItem& rhs)
: m_pMat(rhs.m_pMat->Clone()),
  m_pRes(rhs.m_pRes ? new CResultData(*rhs.m_pRes) : 0)
, m_materialParameterError(rhs.m_materialParameterError)
{
}

CMatParamDlg::CHistoryItem::~CHistoryItem()
{
  const CMaterialHelperFactory *f = CMaterialHelperFactory::Instance();
  ml::CMaterial::CCreator *creator = f->getMatCreator(0); // any will do
  creator->Destroy(m_pMat);
  delete m_pRes;
}

const CLibraryMaterial& CMatParamDlg::CHistoryItem::Material() const
{
  return *m_pMat;
}

const CResultData* CMatParamDlg::CHistoryItem::ResultData() const
{
  return m_pRes;
}

const std::vector <QString>&
  CMatParamDlg::CHistoryItem::materialParameterError() const
{
  return m_materialParameterError;
}
