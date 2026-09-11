// attritetraformation.cpp : implementation file
//

#include "attriformation.h"
#include "MeshBase.h"
#include "geomec.h"
#include "stdafx.h"

#ifdef _DEBUG
// #define new DEBUG_NEW
#ifdef _MSC_VER
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif // _MSC_VER
#endif

/////////////////////////////////////////////////////////////////////////////
// CElementSetObject
/////////////////////////////////////////////////////////////////////////////

CAttriFormation::CElementSetObject::CSizeObject::CSizeObject(CElementSetObject &parent)
    : ISubListObject(parent, 2), m_parent(parent) {}

QString CAttriFormation::CElementSetObject::CSizeObject::Text() const {
  QString sRet("{no mesh}");
  const CModelBase &model = dynamic_cast<const CModelBase &>(m_parent.ElementSet().Model());

  if (model.IsMesh())
    sRet = QString("%1").arg(m_parent.ElementSet().ElementSet().ElementSize());
  return sRet;
}

CAttriFormation::CElementSetObject::CPressureObject::CPressureObject(CElementSetObject &parent)
    : ISubListObject(parent, 1), m_parent(parent) {}

QString CAttriFormation::CElementSetObject::CPressureObject::Text() const {
  QString sRet("Formation");
  return sRet;
}

CAttriFormation::CElementSetObject::CElementSetObject(CListCtrlBase &ctrl, const IFormationElementSet &elementset)
    : IListObject(ctrl), m_elementset(elementset) {
  Ctrl().SetItemData(Ctrl().GetItemCount() - 1, (DWORD_PTR)this);
  new CPressureObject(*this);
  new CSizeObject(*this);
}

const IFormationElementSet &CAttriFormation::CElementSetObject::ElementSet() const { return m_elementset; }

QString CAttriFormation::CElementSetObject::Text() const { return m_elementset.Name(); }

unsigned int CAttriFormation::CElementSetObject::Icon() const { return m_elementset.IconId(); }

/////////////////////////////////////////////////////////////////////////////
// CAttriFormation dialog

CAttriFormation::CAttriFormation(CFormationBase &formation, CWnd *pParent /*=NULL*/)
    : CAttriFormationTemplate<CFormationBase>(CAttriFormation::IDD, formation, pParent),
      m_elementOrderSubDialog(this, Source()) {
  //{{AFX_DATA_INIT(CAttriFormation)
  // NOTE: the ClassWizard will add member initialization here
  //}}AFX_DATA_INIT
}

void CAttriFormation::DoDataExchange(CDataExchange *pDX) {
  CAttriFormationTemplate<CFormationBase>::DoDataExchange(pDX);

  //{{AFX_DATA_MAP(CAttriFormation)
  DDX_Control(pDX, IDC_LC_BODY, m_lcElementSet);
  //}}AFX_DATA_MAP

  m_elementOrderSubDialog.doDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAttriFormation, CDialog)
//{{AFX_MSG_MAP(CAttriFormation)
ON_BN_CLICKED(IDC_ST_COLOR, OnStColor)
ON_WM_PAINT()
//}}AFX_MSG_MAP
ON_BN_CLICKED(IDC_INHERITFROMMODEL, OnBnClickedInheritFromModel)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAttriFormation message handlers

BOOL CAttriFormation::OnInitDialog() {
  CAttriFormationTemplate<CFormationBase>::OnInitDialog();

  CRect rect;
  m_lcElementSet.GetWindowRect(&rect);

  m_lcElementSet.InsertColumn(0, _T("Name"), LVCFMT_LEFT, (rect.Width() - 3) * 0.5, 0);
  m_lcElementSet.InsertColumn(1, _T("Pressure"), LVCFMT_LEFT, (rect.Width() - 3) * 0.3, 1);
  m_lcElementSet.InsertColumn(2, _T("Elements"), LVCFMT_LEFT, (rect.Width() - 3) * 0.2, 2);

  // Fill our list control with volumes
  for (int nVolume = 0; nVolume < Copy().ElementSetSize(); nVolume++) {
    new CElementSetObject(m_lcElementSet, Copy().ElementSet(nVolume));
  }

  m_elementOrderSubDialog.onInitDialog();

  return TRUE; // return TRUE unless you set the focus to a control
               // EXCEPTION: OCX Property Pages should return FALSE
}

void CAttriFormation::OnStColor() {
  Copy().Color(SelectColor(Copy().Color()));
  Invalidate();
  UpdateData(FALSE);
}

void CAttriFormation::OnPaint() {
  CPaintDC dc(this);

  FillFrame(IDC_ST_COLOR, Copy().Color(), dc);
}

void CAttriFormation::OnBnClickedInheritFromModel() { m_elementOrderSubDialog.onBnClickedInheritFromModel(); }

void CAttriFormation::OnOK() {
  CAttriFormationTemplate<CFormationBase>::OnOK();

  m_elementOrderSubDialog.onOK();
}
