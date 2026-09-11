#include "stdafx.h"
#include "Geomec.h"

#include "SelectFormationsDlg.h"
#include "FormationBase.h"
#include "ThinLayerUpscaling.h"
#include "BaseEntryTypes.h"
#include "IFormationFilter.h"

CSelectFormationsDlg::CSelectFormationsDlg(CStorageNode& container, const IFormationFilter *formationFilter/*= 0*/)
: CDialog(IDD_UPSCALING_SELECT_FORMATIONS_DLG),
  m_formationFilter(formationFilter),
  m_container(container)
{
}

void CSelectFormationsDlg::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
  DDX_Control(pDX, IDC_LIST_FORMATIONS, m_lbFormations);
}

BOOL CSelectFormationsDlg::OnInitDialog()
{
  CDialog::OnInitDialog();

	m_lbFormations.InsertColumn(0, "Formation", LVCFMT_LEFT, 230);

  const TFormationBaseEntry* pEntry = static_cast<const TFormationBaseEntry*>(m_container.Model().GraphEntry(MD_BASE_FORMATION));
  assert(pEntry);
  const TFormationBaseEntry::TSortedNodeSet& stNodes = pEntry->SortedEntryNodes();
  TFormationBaseEntry::TSortedNodeSet::const_iterator it;
  for(it = stNodes.begin(); it != stNodes.end(); ++it)
  {
    const CFormationBase& formation = **it;
    if( !m_formationFilter || m_formationFilter->FormationValid(formation) )
    {
      if(m_container.IsLinkedTo(formation))
        m_stFormations.insert(&formation);
      new CFormationObserver(formation, *this, m_lbFormations);
    }
  }

  return TRUE;
}

void CSelectFormationsDlg::OnOK()
{
  COperation oper(m_container.Model());

  // link to checked formations
  TFormationSet::iterator it;
  for(it = m_stFormations.begin(); it != m_stFormations.end(); ++it)
  {
    if(!m_container.IsLinkedTo(**it))
      m_container.LinkTo(const_cast<CFormationBase&>(**it));
  }

  // unlink unchecked formations
  for (size_t i = 0; i < m_container.referenceSize(); ++i)
  {
    const CFormationBase* pFormation = dynamic_cast<const CFormationBase*>(&m_container.referenceAt(i));
    if(pFormation && m_stFormations.find(pFormation) == m_stFormations.end())
      m_container.UnLink(const_cast<CFormationBase&>(*pFormation));
  }

  CDialog::OnOK();
}


BEGIN_MESSAGE_MAP(CSelectFormationsDlg, CDialog)
  ON_BN_CLICKED(IDC_SELECT_ALL, &CSelectFormationsDlg::OnBnClickedSelectAll)
  ON_BN_CLICKED(IDC_SELECT_NONE, &CSelectFormationsDlg::OnBnClickedSelectNone)
  ON_BN_CLICKED(IDC_INVERT, &CSelectFormationsDlg::OnBnClickedInvert)
END_MESSAGE_MAP()

void CSelectFormationsDlg::OnBnClickedSelectAll()
{
  const TFormationBaseEntry* pEntry = static_cast<const TFormationBaseEntry*>(m_container.Model().GraphEntry(MD_BASE_FORMATION));
  assert(pEntry);
  const TFormationBaseEntry::TNodeSet& stNodes = pEntry->EntryNodes();
  TFormationBaseEntry::TNodeSet::const_iterator it;
  for(it = stNodes.begin(); it != stNodes.end(); ++it)
    m_stFormations.insert(*it);

  UpdateFormationsListBox();
  UpdateData(FALSE);
}

void CSelectFormationsDlg::OnBnClickedSelectNone()
{
  m_stFormations.clear();

  UpdateFormationsListBox();
  UpdateData(FALSE);
}

void CSelectFormationsDlg::OnBnClickedInvert()
{
  const TFormationBaseEntry* pEntry = static_cast<const TFormationBaseEntry*>(m_container.Model().GraphEntry(MD_BASE_FORMATION));
  assert(pEntry);
  const TFormationBaseEntry::TNodeSet& stNodes = pEntry->EntryNodes();
  TFormationBaseEntry::TNodeSet::const_iterator it;
  for(it = stNodes.begin(); it != stNodes.end(); ++it)
  {
    TFormationSet::iterator itf = m_stFormations.find(*it);
    if(itf == m_stFormations.end())
      m_stFormations.insert(*it);
    else
      m_stFormations.erase(itf);
  }

  UpdateFormationsListBox();
  UpdateData(FALSE);
}

void CSelectFormationsDlg::UpdateFormationsListBox()
{
  int i;
  for(i = 0; i < m_lbFormations.GetItemCount(); ++i)
  {
    IListObject* pListObject = (IListObject*)m_lbFormations.GetItemData(i);
    pListObject->Update();
  }
}

//// CFormationObserver
CSelectFormationsDlg::CFormationObserver::CFormationObserver(const CFormationBase& formation, CSelectFormationsDlg& dlg, CListCtrl& ctrl)
: IListObject(ctrl),
  m_formation(formation),
  m_dlg(dlg)
{
  ctrl.SetItemData(ctrl.GetItemCount() - 1, (DWORD_PTR) this);
  Update();
}

unsigned int CSelectFormationsDlg::CFormationObserver::Icon() const
{
  return m_formation.IconId();
}

QString CSelectFormationsDlg::CFormationObserver::Text() const
{
  return m_formation.Name();
}

unsigned int CSelectFormationsDlg::CFormationObserver::StateIcon() const
{
  if(m_dlg.m_stFormations.find(&m_formation) != m_dlg.m_stFormations.end())
    return IDI_CHECKED;
  return IDI_UNCHECKED;
}

void CSelectFormationsDlg::CFormationObserver::ToggleState()
{
  CSelectFormationsDlg::TFormationSet::iterator it = m_dlg.m_stFormations.find(&m_formation);
  if(it != m_dlg.m_stFormations.end())
    m_dlg.m_stFormations.erase(it);
  else
    m_dlg.m_stFormations.insert(&m_formation);
  Update();
  m_dlg.UpdateData();
}


///// CSelectFormationsSetNode
CSelectFormationsSetNode::CSelectFormationsSetNode(TFormationsSet& stFormations, CFemAppModel& model)
: CStorageNode(model),
  m_stFormations(stFormations)
{
  TFormationsSet::const_iterator it;
  for(it = stFormations.begin(); it != stFormations.end(); ++it)
    LinkTo(const_cast<CFormationBase&>(**it));
}

void CSelectFormationsSetNode::OnNewNeighbour(const CGraphNode& node)
{
  const CFormationBase* pFor = dynamic_cast<const CFormationBase*>(&node);
  if(pFor)
    m_stFormations.insert(pFor);
}

void CSelectFormationsSetNode::OnNeighbourDeleted(const CGraphNode& node)
{
  const CFormationBase* pFor = dynamic_cast<const CFormationBase*>(&node);
  if(pFor)
    m_stFormations.erase(pFor);
}

unsigned int CSelectFormationsSetNode::IconId() const
{
  return 0;
}

unsigned int CSelectFormationsSetNode::TypeId() const
{
  return 0;
}

bool CSelectFormationsSetNode::Empty() const
{
  return true;
}

long CSelectFormationsSetNode::SavedItems() const
{
  return 0;
}
