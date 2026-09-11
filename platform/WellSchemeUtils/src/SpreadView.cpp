// SpreadView.cpp : implementation file
//

#include "stdafx.h"
#include "wellpathbase.h"
#include "SpreadView.h"
#include "SSOCX.H"

#ifdef _DEBUG
//#define new DEBUG_NEW
#ifdef _MSC_VER
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif  // _MSC_VER
#endif

#define SPREAD_COL_TMD 1
#define SPREAD_COL_TVD 2
#define SPREAD_COL_INC 3
#define SPREAD_COL_AZI 4
#define SPREAD_COL_DSL 5
#define SPREAD_COL_NOR 6
#define SPREAD_COL_EAS 7

#define SPREAD_COL_NR 7
#define SPREAD_FIRST_ROW 1
#define SPREAD_LOCK_COLOR  RGB(127,127,127) //0x000000FF
#define SPREAD_DEFAULT_COLOR RGB(255,255,255)

#define SPREAD_KEYCODE_DEL 46
#define SPREAD_KEYCODE_ENTER 13


/////////////////////////////////////////////////////////////////////////////
// CSpreadView

IMPLEMENT_DYNCREATE(CSpreadView, CFormView)

CSpreadView::CSpreadView()
  : CFormView(CSpreadView::IDD)
{
  m_Us=QU::SI_UNIT;
  m_Type = TMD_INC;
  m_Azimuth = true;
  m_CellChanged=false;
  m_Dirty=false;
  //{{AFX_DATA_INIT(CSpreadView)
    // NOTE: the ClassWizard will add member initialization here
  //}}AFX_DATA_INIT
}

CSpreadView::~CSpreadView()
{
}

void CSpreadView::DoDataExchange(CDataExchange* pDX)
{
  CFormView::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CSpreadView)
  
  //}}AFX_DATA_MAP

  //DDX_Control(pDX, IDC_SPREAD1, m_SpreadSheet);
  //m_SpreadSheet.ShowWindow(TRUE);

}


BEGIN_MESSAGE_MAP(CSpreadView, CFormView)
  //{{AFX_MSG_MAP(CSpreadView)
  ON_WM_SIZE()
  ON_COMMAND(ED_ID_3DV_COPYPICTURE, OnId3dvCopypicture)
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSpreadView diagnostics

#ifdef _DEBUG
void CSpreadView::AssertValid() const
{
  CFormView::AssertValid();
}

void CSpreadView::Dump(CDumpContext& dc) const
{
  CFormView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CSpreadView message handlers

void CSpreadView::OnSize(unsigned int nType, int cx, int cy) 
{
  CFormView::OnSize(nType, cx, cy);

  if(!GetDlgItem(IDC_SPREAD1))
    return;

  if(!IsWindow(GetDlgItem(IDC_SPREAD1)->m_hWnd))
    return;


  GetDlgItem(IDC_SPREAD1)->MoveWindow(0,0,cx,cy);
  
}



BEGIN_EVENTSINK_MAP(CSpreadView, CFormView)
  //{{AFX_EVENTSINK_MAP(CSpreadView)
  ON_EVENT(CSpreadView, IDC_SPREAD1, 17 /* LeaveCell */, OnLeaveCell, VTS_I4 VTS_I4 VTS_I4 VTS_I4 VTS_PBOOL)
  ON_EVENT(CSpreadView, IDC_SPREAD1, -602 /* KeyDown */, OnKeyDown, VTS_PI2 VTS_I2)
  ON_EVENT(CSpreadView, IDC_SPREAD1, 18 /* LeaveRow */, OnLeaveRow, VTS_I4 VTS_BOOL VTS_BOOL VTS_BOOL VTS_I4 VTS_I4 VTS_PBOOL)
  ON_EVENT(CSpreadView, IDC_SPREAD1, 4 /* Change */, OnChangeSpread, VTS_I4 VTS_I4)
  ON_EVENT(CSpreadView, IDC_SPREAD1, 22 /* RightClick */, OnRightClickSpread, VTS_I2 VTS_I4 VTS_I4 VTS_I4 VTS_I4)
  ON_EVENT(CSpreadView, IDC_SPREAD1, 3 /* ButtonClicked */, OnButtonClickedSpread, VTS_I4 VTS_I4 VTS_I2)
  ON_EVENT(CSpreadView, IDC_SPREAD1, 11 /* DblClick */, OnDblClickSpread, VTS_I4 VTS_I4)
  //}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()


void CSpreadView::OnInitialUpdate() 
{
  CFormView::OnInitialUpdate();
}



void CSpreadView::Init(well::CWellPathBase& wellpath, QU::UNIT us)
{
  m_Us=us;
  m_pWellpath = &wellpath;
  CWellSpreadSheet* pSpread = (CWellSpreadSheet*)GetDlgItem(IDC_SPREAD1);

  QString frmt("%1 [%2]");
  
  assert(pSpread);

  pSpread->SetRow(0);

  pSpread->SetCol(SPREAD_COL_TMD);
  pSpread->SetText(frmt.arg("TMD", CLengthQuantity().UnitName(m_Us).c_str()).toStdString().c_str());
  
  pSpread->SetCol(SPREAD_COL_TVD);
  pSpread->SetText("TVD");

  pSpread->SetCol(SPREAD_COL_INC);
  pSpread->SetText("Inclination");

  pSpread->SetCol(SPREAD_COL_AZI);
  pSpread->SetText("Azimuth");

  pSpread->SetCol(SPREAD_COL_DSL);
  pSpread->SetText(frmt.arg("DLS", CCurvatureQuantity().UnitName(m_Us).c_str()).toStdString().c_str());

  pSpread->SetCol(SPREAD_COL_NOR);
  pSpread->SetText("North departure");

  pSpread->SetCol(SPREAD_COL_EAS);
  pSpread->SetText("East departure");
  pSpread->SetColWidth(30,2);

  pSpread->SetLockBackColor(SPREAD_LOCK_COLOR);
  pSpread->SetAutoClipboard(TRUE);

  pSpread->SetCol(SPREAD_COL_NOR);
  pSpread->SetRow(-1);
  pSpread->SetLock(TRUE);

  pSpread->SetCol(SPREAD_COL_EAS);
  pSpread->SetRow(-1);
  pSpread->SetLock(TRUE);

  pSpread->SetCol(SPREAD_COL_DSL);
  pSpread->SetRow(-1);
  pSpread->SetLock(TRUE);

  UpdateEnabledCells();
  UpdateCells();
  
}

double String2Double(CString& str)
{
  assert(!str.IsEmpty());
  char *stopstring=0;
  str.TrimLeft();
  str.TrimRight();
  double dTmp = strtod(str,&stopstring);
  assert(*stopstring ==0);
  return dTmp;
}


int CSpreadView::GetNumberOfValidRows()
{
  CWellSpreadSheet* pSpread = (CWellSpreadSheet*)GetDlgItem(IDC_SPREAD1);
  return pSpread->GetDataRowCnt();
}

void CSpreadView::UpdateWellPath()
{
  CWellSpreadSheet* pSpread = (CWellSpreadSheet*)GetDlgItem(IDC_SPREAD1);

  double global_northing=0;
  double global_easting=0;
  double global_depth=0;

  double start_northing=0;
  double start_easting=0;
  double start_depth=0;
  double start_inc=0;

  QVector<double> tvd;
  QVector<double> tmd;
  QVector<double> azi;
  QVector<double> inc;

  int count=0;

  //loop over rows until empty cell is found
  //fill array with values
  while(1)
  {

    double dtmd;
    double dazi;
    double dinc;
    double dtvd;
    double start_azi;

    CString str;
    pSpread->SetRow(count+1);

    if(count==0)
    {
      if(m_Type ==TMD_INC)
      {
        //get first tvd value
        pSpread->SetCol(SPREAD_COL_TVD);
        str = pSpread->GetValue();
        if(str.IsEmpty())
          break;
        
        start_depth = CLengthQuantity(String2Double(str),m_Us).Value();
      }
      else if(m_Type ==TMD_TVD)
      {
        //get first inc value
        pSpread->SetCol(SPREAD_COL_INC);
        str = pSpread->GetValue();
        if(str.IsEmpty())
          break;
        
        start_inc = String2Double(str);
      }
    }

    //get TMD
    pSpread->SetCol(SPREAD_COL_TMD);
    str = pSpread->GetValue();
    if(str.IsEmpty())
      break;
    dtmd = String2Double(str);

    if(m_Azimuth || count==0)
    {
      //get azimuth		
      pSpread->SetCol(SPREAD_COL_AZI);
      str = pSpread->GetValue();
      if(str.IsEmpty())
        break;
      dazi = String2Double(str);

      if(count==0)
        start_azi=dazi;
      
    }
    else
    {
      dazi=start_azi;
    }

    //get inclination or tvd
    if(m_Type ==TMD_INC)
    {
      pSpread->SetCol(SPREAD_COL_INC);
      str = pSpread->GetValue();
      if(str.IsEmpty())
        break;

      dinc = String2Double(str);

      inc.resize(count+1);
      inc[count]=dinc;
    }
    else if(m_Type ==TMD_TVD)
    {
      pSpread->SetCol(SPREAD_COL_TVD);
      str = pSpread->GetValue();
      if(str.IsEmpty())
        break;

      dtvd = String2Double(str);

      dtvd=CLengthQuantity(dtvd,m_Us).Value();
      tvd.resize(count+1);
      tvd[count]=dtvd;
    }

    tmd.resize(count+1);
    azi.resize(count+1);

    dtmd=CLengthQuantity(dtmd,m_Us).Value();
    tmd[count]=dtmd;
    azi[count]=dazi;
    
    count++;
    
  }


  // if array size >=2 then create wellpaths
  if(count >1)
  {
    if(m_Type ==TMD_INC)
    {
      m_pWellpath->InitFromArray_tmd_azi_inc(
                          global_northing,
                          global_easting,
                          global_depth,
                          start_northing,
                          start_easting,
                          start_depth,
                          tmd,
                          azi,
                          inc);
    }
    else if(m_Type ==TMD_TVD)
    {
      m_pWellpath->InitFromArray_tmd_tvd_azi(
                        global_northing,
                     		global_easting,
                        global_depth,
                        start_northing,
                     		start_easting,
                        start_inc,
                        tmd,
                        tvd,
                        azi);

    }
    else
      assert(false);
  }
  else
  {
    m_pWellpath->InvalidateDefPointList();
  }

  UpdateCells();
}

void CSpreadView::ClearColumn(int ColNr,int StartRow /*=1*/)
{
  CWellSpreadSheet* pSpread = (CWellSpreadSheet*)GetDlgItem(IDC_SPREAD1);

  
  pSpread->SetCol(ColNr);
  pSpread->SetCol2(ColNr);
  pSpread->SetRow(StartRow);
  pSpread->SetRow2(pSpread->GetMaxRows());
  pSpread->SetBlockMode(TRUE);
  pSpread->SetAction(SS_ACTION_CLEAR_TEXT);
  pSpread->SetBlockMode(FALSE);
}

void CSpreadView::UpdateCells()
{
  m_CellChanged=false;

  well::CWellPointList::Iterator it =  m_pWellpath->DefPointList().begin();


  CWellSpreadSheet* pSpread = (CWellSpreadSheet*)GetDlgItem(IDC_SPREAD1);
  pSpread->SetReDraw(TRUE);

  long row=SPREAD_FIRST_ROW;
  QString tmp("%1");

  ClearColumn(SPREAD_COL_DSL);
  ClearColumn(SPREAD_COL_EAS);
  ClearColumn(SPREAD_COL_NOR);

  if(!m_Azimuth)
    ClearColumn(SPREAD_COL_AZI,SPREAD_FIRST_ROW+1);

  if(m_Type ==TMD_TVD)
  {
    ClearColumn(SPREAD_COL_INC,SPREAD_FIRST_ROW+1);
  }
  else if(m_Type ==TMD_INC)
  {
    ClearColumn(SPREAD_COL_TVD,SPREAD_FIRST_ROW+1);
  }
  else
    assert(false);

  CCurvatureQuantity curv;
  while(it != m_pWellpath->DefPointList().end())
  {
    well::CWellPoint* p = *it;
    pSpread->SetRow(row++);

    pSpread->SetCol(SPREAD_COL_TMD);
    pSpread->SetValue(tmp.arg(p->TMD().Value(m_Us)).toStdString().c_str());

    pSpread->SetCol(SPREAD_COL_TVD);
    pSpread->SetValue(tmp.arg(p->TVD().Value(m_Us)).toStdString().c_str());

    pSpread->SetCol(SPREAD_COL_INC);
    pSpread->SetValue(tmp.arg(p->Inclination().Value(m_Us)).toStdString().c_str());

    pSpread->SetCol(SPREAD_COL_AZI);
    pSpread->SetValue(tmp.arg(p->Azimuth().Value(m_Us)).toStdString().c_str());

    pSpread->SetCol(SPREAD_COL_NOR);
    pSpread->SetValue(tmp.arg(p->Northing().Value(m_Us)).toStdString().c_str());

    pSpread->SetCol(SPREAD_COL_EAS);
    pSpread->SetValue(tmp.arg(p->Easting().Value(m_Us)).toStdString().c_str());

    pSpread->SetCol(SPREAD_COL_DSL);
    curv.SetValue(m_pWellpath->GetDogLegSeverity(p->TMD().Value()));
    pSpread->SetValue(tmp.arg(curv.Value(m_Us)).toStdString().c_str());

    ++it;
  }

  pSpread->SetReDraw(TRUE);
}

BOOL CSpreadView::OnCmdMsg(unsigned int nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) 
{
  CWellSpreadSheet* pSpread = (CWellSpreadSheet*)GetDlgItem(IDC_SPREAD1);

  if(pSpread && pSpread->OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
    return TRUE;

  return CFormView::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

bool CSpreadView::IsDirty()
{
  return m_Dirty;
}

void CSpreadView::OnKeyDown(short FAR* KeyCode, short Shift) 
{
  

  CWellSpreadSheet* pSpread = (CWellSpreadSheet*)GetDlgItem(IDC_SPREAD1);

  m_CellChanged=true;
  m_Dirty = true;

  if(SPREAD_KEYCODE_DEL == *KeyCode )
  {
    pSpread->SetCol(pSpread->GetSelBlockCol());
    pSpread->SetCol2(pSpread->GetSelBlockCol2());
    pSpread->SetRow(pSpread->GetSelBlockRow());
    pSpread->SetRow2(pSpread->GetSelBlockRow2());

    pSpread->SetReDraw(FALSE);
    pSpread->SetBlockMode(TRUE);
    pSpread->SetAction(SS_ACTION_CLEAR_TEXT);
    pSpread->SetBlockMode(FALSE);
    pSpread->SetReDraw(TRUE);
    UpdateWellPath();
    
  }
  else if(SPREAD_KEYCODE_ENTER == *KeyCode)
  {
    UpdateWellPath();
  }
  
}

void CSpreadView::UpdateAzimuth()
{
  CWellSpreadSheet* pSpread = (CWellSpreadSheet*)GetDlgItem(IDC_SPREAD1);

  if(m_Azimuth)
  {
    pSpread->SetCol(SPREAD_COL_AZI);
    pSpread->SetRow(-1);
    pSpread->SetLock(FALSE);
  }
  else
  {
    pSpread->SetCol(SPREAD_COL_AZI);
    pSpread->SetRow(SPREAD_FIRST_ROW);
    pSpread->SetCol2(SPREAD_COL_AZI);
    pSpread->SetRow2(pSpread->GetMaxRows());
    pSpread->SetRow(SPREAD_FIRST_ROW+1);
    pSpread->SetBlockMode(TRUE);
    pSpread->SetLock(TRUE);
    pSpread->SetBlockMode(FALSE);
  }
}

void CSpreadView::UpdateEnabledCells()
{
  UpdateAzimuth();
  
  CWellSpreadSheet* pSpread = (CWellSpreadSheet*)GetDlgItem(IDC_SPREAD1);
  pSpread->SetReDraw(FALSE);

  //unlock first cell
  pSpread->SetRow(1);
  pSpread->SetLock(FALSE);


  if(m_Type ==TMD_TVD)
  {
    //lock Inclination not first one
    pSpread->SetCol(SPREAD_COL_INC);
    pSpread->SetRow(-1);
    pSpread->SetLock(TRUE);

    //unlock first cell
    pSpread->SetRow(1);
    pSpread->SetLock(FALSE);

    //unlock TVD acept first one
    pSpread->SetCol(SPREAD_COL_TVD);
    pSpread->SetRow(-1);
    pSpread->SetLock(FALSE);


  }
  else if(m_Type ==TMD_INC)
  {
    //lock TVD acept first one
    pSpread->SetCol(SPREAD_COL_TVD);
    pSpread->SetRow(-1);
    pSpread->SetLock(TRUE);

    //unlock first cell
    pSpread->SetRow(1);
    pSpread->SetLock(FALSE);

    //unlock Inclination not first one
    pSpread->SetCol(SPREAD_COL_INC);
    pSpread->SetRow(-1);
    pSpread->SetLock(FALSE);
  }
  else
    assert(false);

  pSpread->SetReDraw(TRUE);

}

void CSpreadView::OnLeaveCell(long Col, long Row, long NewCol, long NewRow, BOOL FAR* Cancel) 
{
  CWellSpreadSheet* pSpread = (CWellSpreadSheet*)GetDlgItem(IDC_SPREAD1);

  if(m_CellChanged)
    UpdateWellPath();

}


void CSpreadView::SetDefType(DEF_TYPE type,bool update)
{
  m_Type=type; 
  if(update)
  {
    UpdateEnabledCells();
    UpdateWellPath();
  }

}

void CSpreadView::SetAzimuth(bool azimuth,bool update)
{
  m_Azimuth=azimuth; 
  if(update)
  {

    UpdateEnabledCells();
    UpdateWellPath();
  }
}

void CSpreadView::OnId3dvCopypicture() 
{
  // TODO: Add your command handler code here
  
}

/*virtual*/ void CSpreadView::OnLeaveRow(long Row,BOOL RowWasLast,BOOL RowChanged,BOOL AllCellsHaveData,long NewRow,long NewRowIsLast,BOOL FAR* Cancel)
{
  // 
}

/*virtual*/ void CSpreadView::OnChangeSpread(long Col,long Row)
{
  // 
}

void CSpreadView::ConvertTwipsToPixels (long& width, long& height)
{
  static  int nLogX = 0;
  static  int nLogY = 0;

  CWellSpreadSheet* pSpread = (CWellSpreadSheet*)GetDlgItem(IDC_SPREAD1);

  if(!nLogX)
  {
     CDC* pDC = pSpread->GetDC();
     nLogX = pDC->GetDeviceCaps(LOGPIXELSX);
     nLogY = pDC->GetDeviceCaps(LOGPIXELSY);
     ReleaseDC(pDC);
  }

  width = width*nLogX/1440;
  height = height*nLogY/1440;

}


/*virtual*/ void CSpreadView::OnRightClickSpread(short ClickType,long Col,long Row,long MouseX,long MouseY)
{
    CWellSpreadSheet* pSpread = (CWellSpreadSheet*)GetDlgItem(IDC_SPREAD1);

    CMenu menu;
    int count=1;
    menu.CreatePopupMenu();
    menu.AppendMenu(MF_STRING|MF_ENABLED,count++,"Copy");	
    menu.AppendMenu(MF_STRING|MF_ENABLED,count++,"Paste");	
    

    ConvertTwipsToPixels(MouseX,MouseY);
    CRect r;
    pSpread->GetWindowRect(r);
    
    //child->invokeContextMenu(r.left+MouseX,r.top+MouseY);

    //menu.EnableMenuItem(0,MF_ENABLED);
    //menu.EnableMenuItem(1,MF_ENABLED);
    unsigned int nCode = menu.TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON |TPM_RETURNCMD|TPM_NONOTIFY ,
          r.left+MouseX, r.top+MouseY, AfxGetMainWnd());

    if(nCode==1)
      pSpread->SetAction(22);
    else if(nCode==2)
    {
      pSpread->SetAction(24);
      UpdateWellPath();
    }
}

/*virtual*/ void CSpreadView::OnButtonClickedSpread(long Col,long Row,short ButtonDown)
{
  // 
}

/*virtual*/ void CSpreadView::OnDblClickSpread(long Col,long Row)
{
  // 
}
