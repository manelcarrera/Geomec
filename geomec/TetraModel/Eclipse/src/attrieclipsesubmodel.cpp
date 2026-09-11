// attrieclipsesubmodel.cpp : implementation file
//

#include "attrieclipsesubmodel.h"
#include "EclipseModel.h"
#include "GlobalMessage.h"
#include "TetraSubHorizon.h"
#include "geomec.h"
#include "resourceIDI.h"
#include "resourceIDS.h"
#include "stdafx.h"

#ifdef _DEBUG
// #define new DEBUG_NEW
#ifdef _MSC_VER
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif // _MSC_VER
#endif

///////////////////////////////////////////////////////////////////////////////////
// CAttriEclipseSubModel::CReservoirTreeObject::CHorizonTreeObject implementation
///////////////////////////////////////////////////////////////////////////////////

CAttriEclipseSubModel::CReservoirTreeObject::CHorizonTreeObject::CHorizonTreeObject(CReservoirTreeObject &parent,
                                                                                    int nHorizonIndex)
    : ITreeObject(parent, TVI_LAST), m_nIndex(nHorizonIndex) {
  Ctrl().SetItemData(Handle(), (DWORD_PTR)this);
  Update();
}

unsigned int CAttriEclipseSubModel::CReservoirTreeObject::CHorizonTreeObject::StateIcon() const {
  if (Dlg().HorizonEnable(m_nIndex))
    return IDI_CHECKED;
  return IDI_UNCHECKED;
}

unsigned int CAttriEclipseSubModel::CReservoirTreeObject::CHorizonTreeObject::Icon() const { return IDI_HORIZON; }

void CAttriEclipseSubModel::CReservoirTreeObject::CHorizonTreeObject::ToggleState() {
  Dlg().HorizonEnable(m_nIndex, !Dlg().HorizonEnable(m_nIndex));
  // Updating is done by the dlg
}

QString CAttriEclipseSubModel::CReservoirTreeObject::CHorizonTreeObject::Text() const {
  // Try to use the existing horizon name
  if (Dlg().Model().Horizon(m_nIndex))
    return Dlg().Model().Horizon(m_nIndex)->Name();

  // Default text
  QString strText;
  strText = QString("Horizon %1").arg(m_nIndex + 1);
  return strText;
}

BOOL CAttriEclipseSubModel::CReservoirTreeObject::CHorizonTreeObject::operator<(const ICtrlObjectBase &object) const {
  const CHorizonTreeObject *pObject = dynamic_cast<const CHorizonTreeObject *>(&object);
  return m_nIndex < pObject->m_nIndex;
}

CAttriEclipseSubModel &CAttriEclipseSubModel::CReservoirTreeObject::CHorizonTreeObject::Dlg() {
  return ((CReservoirTreeObject &)*Parent()).Dlg();
}

const CAttriEclipseSubModel &CAttriEclipseSubModel::CReservoirTreeObject::CHorizonTreeObject::Dlg() const {
  return ((const CReservoirTreeObject &)*Parent()).Dlg();
}

///////////////////////////////////////////////////////////////////////////////////
// CAttriEclipseSubModel::CReservoirTreeObject::CHorizonTreeObject implementation
///////////////////////////////////////////////////////////////////////////////////
CAttriEclipseSubModel::CReservoirTreeObject::CReservoirTreeObject(CAttriEclipseSubModel &dlg, int nReservoirIndex)
    : ITreeObject(dlg.m_tcHorizon, TVI_ROOT, TVI_LAST), m_dlg(dlg), m_nIndex(nReservoirIndex) {
  Ctrl().SetItemData(Handle(), (DWORD_PTR)this);
  Update();
  // Create children
  for (int i = m_dlg.Model().Reservoir(nReservoirIndex).first; i <= m_dlg.Model().Reservoir(nReservoirIndex).second;
       i++) {
    new CHorizonTreeObject(*this, i);
  }
}

int CAttriEclipseSubModel::CReservoirTreeObject::Index() const { return m_nIndex; }

unsigned int CAttriEclipseSubModel::CReservoirTreeObject::Icon() const { return IDI_FORMATION; }

QString CAttriEclipseSubModel::CReservoirTreeObject::Text() const {
  QString sText;
  sText = QString("Reservoir %1").arg(m_nIndex + 1);
  return sText;
}

BOOL CAttriEclipseSubModel::CReservoirTreeObject::operator<(const ICtrlObjectBase &object) const {
  const CReservoirTreeObject *pTreeObject = dynamic_cast<const CReservoirTreeObject *>(&object);
  return m_nIndex < pTreeObject->m_nIndex;
}

CAttriEclipseSubModel &CAttriEclipseSubModel::CReservoirTreeObject::Dlg() { return m_dlg; }

const CAttriEclipseSubModel &CAttriEclipseSubModel::CReservoirTreeObject::Dlg() const { return m_dlg; }

/////////////////////////////////////////////////////////////////////////////
// CAttriEclipseSubModel dialog

CAttriEclipseSubModel::CAttriEclipseSubModel(CEclipseModel &model, CWnd *pParent /*=NULL*/)
    : CDialog(CAttriEclipseSubModel::IDD, pParent), m_model(model), m_vcHorizon(model.HorizonSize()) {
  for (int i = 0; i < m_vcHorizon.size(); i++)
    m_vcHorizon[i] = model.Horizon(i) != 0;

  //{{AFX_DATA_INIT(CAttriEclipseSubModel)
  // NOTE: the ClassWizard will add member initialization here
  //}}AFX_DATA_INIT
}

void CAttriEclipseSubModel::DoDataExchange(CDataExchange *pDX) {
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CAttriEclipseSubModel)
  DDX_Control(pDX, IDC_TC_HORIZON, m_tcHorizon);
  //}}AFX_DATA_MAP

  if (pDX->m_bSaveAndValidate) {
    if (SelectCount() < 2) {
      _m()->msg(IDS_SELECT_TWO_HORIZON);
      pDX->Fail();
    } else
      Validate();
  }
}

int CAttriEclipseSubModel::SelectCount() const {
  int nRet = 0;
  for (size_t i = 0; i < m_vcHorizon.size(); i++)
    if (m_vcHorizon[i])
      nRet++;

  return nRet;
}

void CAttriEclipseSubModel::Validate() {
  for (int i = 0; i < m_vcHorizon.size(); i++)
    m_model.HorizonEnable(i, m_vcHorizon[i]);
}

BOOL CAttriEclipseSubModel::HorizonEnable(int nHorizonIndex) const { return m_vcHorizon[nHorizonIndex]; }

void CAttriEclipseSubModel::HorizonEnable(int nHorizonIndex, BOOL bHorizonEnable) {
  m_vcHorizon[nHorizonIndex] = bHorizonEnable;
  m_tcHorizon.UpdateTree();
}

CEclipseModel &CAttriEclipseSubModel::Model() { return m_model; }

const CEclipseModel &CAttriEclipseSubModel::Model() const { return m_model; }

BEGIN_MESSAGE_MAP(CAttriEclipseSubModel, CDialog)
//{{AFX_MSG_MAP(CAttriEclipseSubModel)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAttriEclipseSubModel message handlers

BOOL CAttriEclipseSubModel::OnInitDialog() {
  CDialog::OnInitDialog();

  // Fill tree
  for (int nReservoir = 0; nReservoir < m_model.ReservoirSize(); nReservoir++) {
    m_tcHorizon.Expand((new CReservoirTreeObject(*this, nReservoir))->Handle(), TVE_EXPAND);
  }

  return TRUE; // return TRUE unless you set the focus to a control
               // EXCEPTION: OCX Property Pages should return FALSE
}
