// attriformationvolume.cpp : implementation file
//

#include "stdafx.h"
#include "geomec.h"
#include "BaseEntryTypes.h"
#include "attriformationelementset.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#ifdef _MSC_VER#undef THIS_FILE
static char THIS_FILE[] = __FILE__;#endif  // _MSC_VER
#endif

/////////////////////////////////////////////////////////////////////////////
// CAttriFormationElementSet dialog


CAttriFormationElementSet::CAttriFormationElementSet(IFormationElementSet& elementset, CWnd* pParent /*=NULL*/)
: CAttributesTemplate<IFormationElementSet>(CAttriFormationElementSet::IDD, elementset, pParent)
{
  //{{AFX_DATA_INIT(CAttriFormationElementSet)
  //}}AFX_DATA_INIT
}


void CAttriFormationElementSet::DoDataExchange(CDataExchange* pDX)
{
  CAttributesTemplate<IFormationElementSet>::DoDataExchange(pDX);
  CString	sName;
  if(!pDX->m_bSaveAndValidate)
  {
    sName = Copy().Name();
  }

  //{{AFX_DATA_MAP(CAttriFormationElementSet)
  DDX_Control(pDX, IDC_CB_FORMATION, m_cbFormation);
  DDX_Text(pDX, IDC_ED_NAME, sName);
  //}}AFX_DATA_MAP

  if(pDX->m_bSaveAndValidate)
  {
    Copy().Name(sName);
  }
}


BEGIN_MESSAGE_MAP(CAttriFormationElementSet, CDialog)
  //{{AFX_MSG_MAP(CAttriFormationElementSet)
  ON_BN_CLICKED(IDC_ST_COLOR, OnChangeColor)
  ON_WM_PAINT()
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAttriFormationElementSet message handlers

void CAttriFormationElementSet::OnChangeColor() 
{
  Copy().Color(SelectColor(Copy().Color()));
  Invalidate();	
}

void CAttriFormationElementSet::OnOK() 
{
  // Do we need to change formation connection ...
  int nIndex = m_cbFormation.GetCurSel();
  ASSERT(nIndex != -1);
  CFormationBase *pCurFormation = (CFormationBase*)(m_cbFormation.GetItemData(nIndex));
  if(pCurFormation != m_pFormation)
  {
    CFormationBase *pCurCopy = new CFormationBase(*pCurFormation);
    CFormationBase *pOldCopy = new CFormationBase(*m_pFormation);

    // Switch to other formation
    pCurCopy->ConnectItem(Copy());

    CAttributesTemplate<IFormationElementSet>::OnOK();

    (*m_pFormation) = (*pOldCopy);
    (*pCurFormation) = (*pCurCopy);

    delete pOldCopy;
    delete pCurCopy;

    m_pFormation->Modified();
    pCurFormation->Modified();
  }
  else
    CAttributesTemplate<IFormationElementSet>::OnOK();
}

BOOL CAttriFormationElementSet::OnInitDialog() 
{
  // Initialize base first
  CAttributesTemplate<IFormationElementSet>::OnInitDialog();
  
  // Fill the combo box with formations
  TFormationBaseEntry *pEntry = dynamic_cast<TFormationBaseEntry*> (Source().Model().GraphEntry(MD_BASE_FORMATION));
  ASSERT(pEntry);
  TFormationBaseEntry::TNodeSet stFormation = pEntry->EntryNodes();

  for(TFormationBaseEntry::TNodeSet::iterator it = stFormation.begin(); it != stFormation.end(); it++)
  {
    CFormationBase& formation = **it;
    // Add the formation to the combo
    int nIndex = m_cbFormation.AddString(formation.Name());
    m_cbFormation.SetItemData(nIndex, (DWORD)(&formation));
    
    // Set selection if this formation is connected to the volume
    if(Source().IsLinkedTo(formation))
    {
      m_pFormation = &formation;
      m_cbFormation.SetCurSel(nIndex);
    }
  }

  
  return TRUE;  // return TRUE unless you set the focus to a control
              // EXCEPTION: OCX Property Pages should return FALSE
}

void CAttriFormationElementSet::OnPaint() 
{
  CPaintDC dc(this); // device context for painting
  FillFrame(IDC_ST_COLOR, Copy().Color(), dc);
}
