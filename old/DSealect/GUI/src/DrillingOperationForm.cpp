// DrillingOperationForm.cpp : implementation file
//

#include "stdafx.h"
#include "dsealect.h"
#include "DrillingOperationForm.h"
#include "treeobject.h"
#include "DSealectGlobal.h"
#include "SurveyReader.h"
#include "CDataModel.h"
#include "CFormationTopography.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#ifdef _MSC_VER#undef THIS_FILE
static char THIS_FILE[] = __FILE__;#endif  // _MSC_VER
#endif




/////////////////////////////////////////////////////////////////////////////
// CDrillingOperationForm

IMPLEMENT_DYNCREATE(CDrillingOperationForm, CBaseFormView)

//##ModelId=3C0DC15D000F
CDrillingOperationForm::CDrillingOperationForm()
	: CBaseFormView(CDrillingOperationForm::IDD)
{
	//{{AFX_DATA_INIT(CDrillingOperationForm)
	m_intDensityDefinition = 0;
	//}}AFX_DATA_INIT
}

//##ModelId=3C0DC15D0157
CDrillingOperationForm::~CDrillingOperationForm()
{
	if(m_toolTip)
		delete m_toolTip;
}

//##ModelId=3C0DC15D013A
void CDrillingOperationForm::DoDataExchange(CDataExchange* pDX)
{

	CBaseFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDrillingOperationForm)
	DDX_Control(pDX, IDC_lbUMudStressGradAtSurface, m_lbUMudStressGradAtSurface);
	DDX_Control(pDX, IDC_lbUMudDensityAtSurface,	m_lbUMudDensityAtSurface);
	DDX_Control(pDX, IDC_lbMudStressGradAtSurface,	m_lbMudStressGradAtSurface);
	DDX_Control(pDX, IDC_lbMudDensityAtSurface,		m_lbMudDensityAtSurface);
	DDX_Control(pDX, IDC_ebMudStressGradAtSurface,	m_debMudStressGradAtSurface);
	DDX_Control(pDX, IDC_ebMudDensityAtSurface,		m_debMudDensityAtSurface);
	DDX_Control(pDX, IDC_ebMudCompressibility,		m_debMudCompressibility);
	DDX_Control(pDX, IDC_ebBoreholeDiameter,		m_debBoreholeDiameter);
	DDX_Radio(pDX, IDC_optDensityDefinition,		m_intDensityDefinition);
  DDX_Radio(pDX, IDC_OptVertical, m_iDeviated);
	//}}AFX_DATA_MAP

  GetDlgItem(IDC_ButtonImport)->EnableWindow(m_iDeviated != 0);
  CString strFileName = GetModel()->SurveyFileName();
  if(strFileName.IsEmpty())
  {
    if(!m_iDeviated)
      strFileName = "<vertical>";
    else if(GetModel()->IsPolyLineCreated())
      strFileName = "<old model>";
    else
      strFileName = "<not valid>";
  }

  GetDlgItem(IDC_LabelFileName)->SetWindowText(strFileName);

	if(!CheckEntry())
		return;

	ValidateQuantity(pDX, &m_debMudDensityAtSurface,		GetModel()->DrillingScheme()->GetDrillingOperation(0)->MudDensityAtSurface(),		CString("MudDensityAtSurface"));
	ValidateQuantity(pDX, &m_debMudCompressibility,			GetModel()->DrillingScheme()->GetDrillingOperation(0)->MudCompressibility(),						CString("MudCompressibility"));
	ValidateQuantity(pDX, &m_debBoreholeDiameter,			GetModel()->DrillingScheme()->GetDrillingOperation(0)->BoreholeDiameter(),			CString("BoreholeDiameter"));
	
	if (GetModel()->DrillingScheme()->GetDrillingOperation(0)->MudDensityAtSurface()->Undefined())
	{
		m_debMudStressGradAtSurface.MakeEmpty();
	}
	else
	{
		//calculate MudStressGradient from MudDensity
		double dMudStressGrad=GetModel()->DrillingScheme()->GetDrillingOperation(0)->MudDensityAtSurface()->Value();
		CStressGradientQuantity	tempSGQ;
		tempSGQ.Value(dMudStressGrad*ACCELERATION_OF_GRAVITY,CDoubleQuantity::SI_UNIT);
		ShowQuantity(&m_debMudStressGradAtSurface,&tempSGQ);
	}

	if (pDX->m_bSaveAndValidate)
	{
		//from ctrls to data
	}
	else
	{
		//from data to ctrls
		//m_intDensityDefinition=0; //this optionbutton is always set, because the boolean is not stored in data
		
		
		//temporary quantities only needed for setting unit string
		CDensityQuantity			tempDQ;
		CLengthQuantity				tempLQ;
		CSmallLengthQuantity		tempSLQ;
		CCompressibilityQuantity	tempCQ;
		CStressGradientQuantity		tempSQ;
		
		SetUnitlabel(IDC_lbUMudDensityAtSurface,	&tempDQ);
		SetUnitlabel(IDC_lbUMudStressGradAtSurface,	&tempSQ);
		SetUnitlabel(IDC_lbUBoreholeDiameter,		&tempSLQ);
		SetUnitlabel(IDC_lbUMudCompressibility,		&tempCQ); 

    int nShowWarning = SW_HIDE;
	  if(GetModel()->IsPolyLineCreated())
    {
      const geo::IPoint& pt = GetModel()->PolyLine().Point(GetModel()->PolyLine().PointSize() - 1);
      if(pt.Z() <= GetModel()->FormationTopography()->Overburden()->OverburdenBottomTVD()->Value())
        nShowWarning = SW_SHOW;
    }

    GetDlgItem(IDC_ExclTrajectoryWarning)->ShowWindow(nShowWarning);
    GetDlgItem(IDC_LblTrajectoryWarning)->ShowWindow(nShowWarning);
	}
	
	m_lbMudDensityAtSurface.EnableWindow(m_intDensityDefinition==0);
	m_lbUMudDensityAtSurface.EnableWindow(m_intDensityDefinition==0);
	m_debMudDensityAtSurface.EnableWindow(m_intDensityDefinition==0);
	
	m_lbMudStressGradAtSurface.EnableWindow(m_intDensityDefinition==1);
	m_lbUMudStressGradAtSurface.EnableWindow(m_intDensityDefinition==1);
	m_debMudStressGradAtSurface.EnableWindow(m_intDensityDefinition==1);

	DrawView()->Invalidate();
	TreeEntry()->UpdateEnable();
	UpdateButtons();
}


BEGIN_MESSAGE_MAP(CDrillingOperationForm, CBaseFormView)
	//{{AFX_MSG_MAP(CDrillingOperationForm)
	ON_BN_CLICKED(IDC_btnNext, OnNext)
	ON_BN_CLICKED(IDC_optDensityDefinition,			OnoptDensityDefinition)
	ON_BN_CLICKED(IDC_optStressGradientDefinition,	OnoptStressGradientDefinition)
  ON_BN_CLICKED(IDC_OptVertical, OnOptVertical)
  ON_BN_CLICKED(IDC_OptDeviated, OnOptDeviated)
  ON_BN_CLICKED(IDC_ButtonImport, OnImport)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDrillingOperationForm diagnostics

#ifdef _DEBUG
//##ModelId=3C0DC15D0159
void CDrillingOperationForm::AssertValid() const
{
	CBaseFormView::AssertValid();
}

//##ModelId=3C0DC15D0168
void CDrillingOperationForm::Dump(CDumpContext& dc) const
{
	CBaseFormView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CDrillingOperationForm message handlers

//##ModelId=3C0DC15D0177
void CDrillingOperationForm::OnNext() 
{
	// TODO: Add your control notification handler code here
	
}


//##ModelId=3C0DC15D0138
void CDrillingOperationForm::OnInitialUpdate() 
{
	CBaseFormView::OnInitialUpdate();
  m_iDeviated = GetModel()->IsDeviated();
	GetDlgItem(IDC_fraWellPathWithinModelRange)->SetFont(GetFont(TRUE));
	GetDlgItem(IDC_fraWellGeometry)->SetFont(GetFont(TRUE));
	GetDlgItem(IDC_fraDrillingFluidWeight)->SetFont(GetFont(TRUE));

	// TOOLTIP
	static short bInit=0;
	if(!bInit)
	{
		//One time initialization!!
		bInit=1;
		
		m_toolTip.Create(this);
		CRect rect;
		CString sTmp;
		sTmp.LoadString(IDTT_DRO_ebBoreholeDiameter);
		m_toolTip.AddTool( GetDlgItem(IDC_ebBoreholeDiameter), sTmp);
		GetDlgItem(IDC_ebBoreholeDiameter)->GetWindowRect(&rect);	
		ScreenToClient(rect);
		sTmp.LoadString(IDTT_DRO_fraDrillingFluidWeight);
		m_toolTip.AddTool( GetDlgItem(IDC_fraDrillingFluidWeight), sTmp);
		GetDlgItem(IDC_fraDrillingFluidWeight)->GetWindowRect(&rect);	
		ScreenToClient(rect);

		CRect rectMargin(TOOL_TIP_MARGE,TOOL_TIP_MARGE,TOOL_TIP_MARGE,TOOL_TIP_MARGE); //left, top, right, bottom
		m_toolTip.SetMargin(rectMargin);
		m_toolTip.Activate(TRUE);
	}
	
}

//##ModelId=3C0DC15D01C6
void CDrillingOperationForm::OnoptDensityDefinition() 
{
	UpdateData();	
	UpdateData(FALSE);
	GetDocument()->SetModifiedFlag(TRUE);

}

//##ModelId=3C0DC15D01D4
void CDrillingOperationForm::OnoptStressGradientDefinition() 
{
	UpdateData();	
	UpdateData(FALSE);
	GetDocument()->SetModifiedFlag(TRUE);
}

//##ModelId=3C0DC15D0010
BOOL CDrillingOperationForm::CheckComplete() const
{
	if(!TreeEntry()->Previous()->Previous()->FormView()->CheckComplete())
	{
		return FALSE;
	}

	if(GetModel()->FormationTopography()->NrOfFormations()<1)
	{
		return FALSE;
	}

	if(!GetModel()->IsPolyLineCreated())
		return FALSE;

  const geo::IPoint& pt = GetModel()->PolyLine().Point(GetModel()->PolyLine().PointSize() - 1);
  if(pt.Z() <= GetModel()->FormationTopography()->Overburden()->OverburdenBottomTVD()->Value())
    return FALSE;

	if(GetModel()->DrillingScheme()->GetDrillingOperation(0)->MudDensityAtSurface()->Undefined())
		return FALSE;

	if(GetModel()->DrillingScheme()->GetDrillingOperation(0)->MudCompressibility()->Undefined())
		return FALSE;

	if(GetModel()->DrillingScheme()->GetDrillingOperation(0)->BoreholeDiameter()->Undefined())
		return FALSE;

	return TRUE;

}

//##ModelId=3CA011A6034B
LRESULT CDrillingOperationForm::OnValidateDoubleEdit(WPARAM id, LPARAM lparam)
{
	m_iControlToValidate=id;
	TRACE("\nIDC_ebMudStressGradAtSurface  = %i\n",IDC_ebMudStressGradAtSurface);
	if(IDC_ebMudStressGradAtSurface == m_iControlToValidate)
	{
		if (m_debMudStressGradAtSurface.IsEmpty())
		{
			m_debMudDensityAtSurface.MakeEmpty();
			UpdateData();
			return TRUE;
		}
		
		CStressGradientQuantity	tempSGQ;
		tempSGQ.Value(m_debMudStressGradAtSurface.GetValue(),GetDoc()->UnitSystem());
		
		double dbDensity=tempSGQ.Value()/ACCELERATION_OF_GRAVITY;
		CDensityQuantity tempDQ;
		tempDQ.Value(dbDensity,CDoubleQuantity::SI_UNIT);

		m_debMudDensityAtSurface.ShowValue(tempDQ.Value(GetDoc()->UnitSystem()));

		UpdateData();
		UpdateData(FALSE); // extra update needed to get old value back when copied value out of range
		m_iControlToValidate=0;
		return TRUE;
	}

	UpdateData();
	m_iControlToValidate=0;
	return TRUE;
}

void CDrillingOperationForm::OnOptVertical()
{
  if(!m_iDeviated)
    return;

  if(GetModel()->IsPolyLineCreated() && AfxMessageBox("Do you want to replace the current well trajectory?", MB_YESNO) == IDNO)
  {
    UpdateData(FALSE);
    return;
  }

  // create a single section
  GetModel()->DrillingScheme()->RemoveAllDrillingDirectionSections();
  GetModel()->DrillingScheme()->AddDrillingDirectionSection();
  GetModel()->DrillingScheme()->GetDrillingDirectionSection(0)->AzimuthWithNorth()->SetValue(0);
  GetModel()->DrillingScheme()->GetDrillingDirectionSection(0)->WellInclination()->SetValue(0);
  GetModel()->DrillingScheme()->GetDrillingDirectionSection(0)->DrillingSectionTopTMD()->SetValue(0);
  GetModel()->DrillingScheme()->GetDrillingDirectionSection(0)->DrillingSectionBottomTMD()->SetValue(
    GetModel()->GetModelBottomTVD().Value());

  GetModel()->SurveyFileName(CString());
  if(GetModel()->CanCreatePolyLine())
    GetModel()->CreatePolyLine();
	UpdateData();	
	UpdateData(FALSE);
	GetDocument()->SetModifiedFlag(TRUE);
}

void CDrillingOperationForm::OnOptDeviated()
{
  if(m_iDeviated)
    return;

  GetModel()->InvalidatePolyLine();
	UpdateData();	
	UpdateData(FALSE);
	GetDocument()->SetModifiedFlag(TRUE);
}

void CDrillingOperationForm::OnImport()
{
  if(GetModel()->IsPolyLineCreated() && AfxMessageBox("Do you want to replace the current well trajectory?", MB_YESNO) == IDNO)
    return;

  CFileDialog dlg(TRUE, 0, 0, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, "Survey Files (*.mwr;*.txt)|*.mwr;*.txt|All Files (*.*)|*.*||");
  if(dlg.DoModal() == IDOK)
  {
    CString strWellPathFile = dlg.GetPathName();
    CSurveyReader sr(*GetModel());
    if(sr.Import(strWellPathFile))
    {
      GetModel()->SurveyFileName(strWellPathFile);
      if(GetModel()->CanCreatePolyLine())
      {
        GetModel()->CreatePolyLine();
        // check whether the last point is below the overburden
        const geo::IPoint& pt = GetModel()->PolyLine().Point(GetModel()->PolyLine().PointSize() - 1);
        if(pt.Z() <= GetModel()->FormationTopography()->Overburden()->OverburdenBottomTVD()->Value())
        {
          AfxMessageBox("The imported well trajectory does not run below the overburden.");
        }
      }
      UpdateData(FALSE); // update label
     	GetDocument()->SetModifiedFlag(TRUE);
   }
  }
}
