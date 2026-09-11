// meshdepthdlg.cpp : implementation file
//

#include "stdafx.h"
#include "geomec.h"
#include "hexameshdepthdlg.h"
#include "HexaEntryTypes.h"
#include "HexaModel.h"
#include "BranchState.h"
#include "GlobalMessage.h"

#ifdef _DEBUG
//#define new DEBUG_NEW
#ifdef _MSC_VER
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif  // _MSC_VER
#endif

CHexaMeshDepthDlg::CFormationListObject::CFormationListObject(CListCtrl& ctrl, CHexaFormation& formation)
: IListObject(ctrl), m_formation(formation)
{
  Ctrl().SetItemData(Ctrl().GetItemCount() - 1, (DWORD_PTR) this);
  new CMeshDepthObject(*this);
}

QString CHexaMeshDepthDlg::CFormationListObject::Text() const
{
  return m_formation.Name();
}

unsigned int CHexaMeshDepthDlg::CFormationListObject::Icon() const
{
  return m_formation.IconId();
}

const CHexaFormation& CHexaMeshDepthDlg::CFormationListObject::Formation() const
{
  return m_formation;
}

CHexaFormation& CHexaMeshDepthDlg::CFormationListObject::Formation()
{
  return m_formation;
}

void CHexaMeshDepthDlg::CFormationListObject::Apply()
{
  CMeshDepthObject* pObject = (CMeshDepthObject*) (SubObject(1));
  Formation().Elements(pObject->Elements());
}

BOOL CHexaMeshDepthDlg::CFormationListObject::Modified() const
{
  CMeshDepthObject* pObject = (CMeshDepthObject*) (SubObject(1));
  return Formation().Elements() != pObject->Elements();
}

BOOL CHexaMeshDepthDlg::CFormationListObject::operator<(const ICtrlObjectBase &object) const
{
  const CFormationListObject *pObject = (const CFormationListObject*)(&object);
  return pObject->Formation().Less(Formation());
}

CHexaMeshDepthDlg::CMeshDepthObject::CMeshDepthObject(CFormationListObject &list_object)
: ISubListObject(list_object, 1)
{
  m_nElement = list_object.Formation().Elements();
}

int CHexaMeshDepthDlg::CMeshDepthObject::Elements() const
{
  return m_nElement;
}

QString CHexaMeshDepthDlg::CMeshDepthObject::Text() const
{
  QString sRet;
  sRet = QString("%1").arg(m_nElement);
  return sRet;
}

BOOL CHexaMeshDepthDlg::CMeshDepthObject::CanEditText() const
{
  return !(static_cast<const CModelBase&>((static_cast<const CFormationListObject&>(ListObject()).Formation().Model()))).BranchState().IsBranch();
}

BOOL CHexaMeshDepthDlg::CMeshDepthObject::EditText(const QString& strText)
{
  if(!IsInteger(strText.toStdString().c_str()))
  {
    _m()->msg("Mesh Depth is not an integer");
    return FALSE;
  }
  
  if( 0 >= atoi(strText.toStdString().c_str()))
  {
    _m()->msg("Mesh depth > 0");
    return FALSE;
  }

  m_nElement = atoi(strText.toStdString().c_str());

  return TRUE;
}


BOOL CHexaMeshDepthDlg::CMeshDepthObject::IsInteger(const CString& sString) const
{
  // Integer
  for(int i = 0; i < sString.GetLength(); i++)
  {
    if(!isdigit(sString[i])) return FALSE;
  }

  return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CHexaMeshDepthDlg dialog


CHexaMeshDepthDlg::CHexaMeshDepthDlg(CFemAppModel& model, CWnd* pParent /*=NULL*/)
  : CDialog(CHexaMeshDepthDlg::IDD, pParent), m_model(model)
{
  //{{AFX_DATA_INIT(CHexaMeshDepthDlg)
    // NOTE: the ClassWizard will add member initialization here
  //}}AFX_DATA_INIT
}


void CHexaMeshDepthDlg::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CHexaMeshDepthDlg)
  DDX_Control(pDX, IDC_LC_FORMATION, m_lcFormation);
  //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CHexaMeshDepthDlg, CDialog)
  //{{AFX_MSG_MAP(CHexaMeshDepthDlg)
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHexaMeshDepthDlg message handlers

BOOL CHexaMeshDepthDlg::OnInitDialog() 
{
  CDialog::OnInitDialog();
  
  CRect rect;
  m_lcFormation.GetWindowRect(&rect);

  m_lcFormation.InsertColumn(0, _T("Formation"), LVCFMT_LEFT,(rect.Width()-3) * 0.7, 0);
  m_lcFormation.InsertColumn(1, _T("#Elements"), LVCFMT_LEFT,(rect.Width()-3) * 0.3, 1);

  std::vector<CHexaFormation*> vcSortedFormations;
  CHexaFormationEntry *pEntry = (CHexaFormationEntry*) m_model.GraphEntry(MD_HEXA_FORMATION);
  CHexaFormationEntry::TNodeSet stFormation = pEntry->EntryNodes();
  CHexaFormationEntry::TNodeSet::iterator it;
  if(!stFormation.empty())
  {
    CHexaFormation *pForm = *stFormation.begin();
    while(pForm->UpperFormation()) pForm = const_cast<CHexaFormation*>(pForm->UpperFormation());
    assert(pForm);
    while(pForm)
    {
      vcSortedFormations.push_back(pForm);
      pForm = const_cast<CHexaFormation*>(pForm->LowerFormation());
    }

    for(size_t i = 0; i < vcSortedFormations.size(); i++)
    {
      new CFormationListObject(m_lcFormation, *vcSortedFormations[i]);
    }
  }

  return TRUE;  // return TRUE unless you set the focus to a control
                // EXCEPTION: OCX Property Pages should return FALSE
}

void CHexaMeshDepthDlg::OnOK()
{
  BOOL bModified = FALSE;
  for(int i = 0; i < m_lcFormation.GetItemCount(); i++)
  {
    CFormationListObject *pObject = (CFormationListObject*) (m_lcFormation.GetItemData(i));
    assert(pObject);
    if(pObject->Modified())
      bModified = TRUE;

    pObject->Apply();		
  }

  if(bModified)
    ((CHexaModel&)m_model).InvalidateMesh();

  CDialog::OnOK();
}
