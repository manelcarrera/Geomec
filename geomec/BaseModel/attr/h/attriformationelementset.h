#if !defined(AFX_ATTRIFORMATIONELEMENTSET_H__8298992F_57D7_4E5E_A561_D5E204717869__INCLUDED_)
#define AFX_ATTRIFORMATIONELEMENTSET_H__8298992F_57D7_4E5E_A561_D5E204717869__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BaseEntryTypes.h"
#include "AttributesTemplate.h"
#include "BranchState.h"

class CFormationBase;

template<class T>
class CAttriFormationElementSet : public CAttributesTemplate<T>
{
  CFormationBase* m_pFormation;
  CComboBox	m_cbFormation;
public:
  CAttriFormationElementSet(unsigned int uDialogTemplate, T& elementset, CWnd* pParent = NULL)   // standard constructor
  : CAttributesTemplate<T>(uDialogTemplate, elementset, pParent) {}

  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
  void OnChangeColor();
  virtual void OnOK();
  virtual BOOL OnInitDialog();
  virtual BOOL OnWndMsg(unsigned int message, WPARAM wParam, LPARAM lParam, LRESULT* pResult);
  virtual BOOL OnCommand( WPARAM wParam, LPARAM lParam );
  void OnPaint();
};

template<class T>
void CAttriFormationElementSet<T>::DoDataExchange(CDataExchange* pDX)
{
  CAttributesTemplate<T>::DoDataExchange(pDX);
  CString	sName;
  if(!pDX->m_bSaveAndValidate)
  {
    sName = Copy().Name().toStdString().c_str();
    if((static_cast<const CModelBase&>(Copy().Model())).BranchState().IsBranch())
      GetDlgItem(IDC_CB_FORMATION)->EnableWindow(FALSE);
  }

  DDX_Control(pDX, IDC_CB_FORMATION, m_cbFormation);
  DDX_Text(pDX, IDC_ED_NAME, sName);

  if(pDX->m_bSaveAndValidate)
  {
    Copy().Name((LPCSTR) sName);
  }
}

template<class T>
BOOL CAttriFormationElementSet<T>::OnWndMsg(unsigned int message, WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
  if(message == WM_PAINT)
  {
    OnPaint();
    return TRUE;
  }

  return CAttributesTemplate<T>::OnWndMsg(message, wParam, lParam, pResult);
}
 
template<class T>
BOOL CAttriFormationElementSet<T>::OnCommand( WPARAM wParam, LPARAM lParam )
{
  unsigned int nID = wParam;
  
  if(nID == IDC_ST_COLOR)
  {
    OnChangeColor();
    return TRUE;
  }

  return CAttributesTemplate<T>::OnCommand(wParam, lParam );
}

template<class T>
void CAttriFormationElementSet<T>::OnChangeColor() 
{
  Copy().Color(SelectColor(Copy().Color()));
  Invalidate();	
}

template<class T>
void CAttriFormationElementSet<T>::OnOK() 
{
  // save if needed, and get rid op copy
  CAttributesTemplate<T>::OnOK();

  // Do we need to change formation connection ...
  int nIndex = m_cbFormation.GetCurSel();
  assert(nIndex != -1);
  CFormationBase *pCurFormation = (CFormationBase*)(m_cbFormation.GetItemData(nIndex));
  if(pCurFormation != m_pFormation)
  {
    pCurFormation->ConnectItem(Source());
  }
}

template<class T>
BOOL CAttriFormationElementSet<T>::OnInitDialog() 
{
  // Initialize base first
  CAttributesTemplate<T>::OnInitDialog();
  
  // Fill the combo box with formations
  TFormationBaseEntry *pEntry = dynamic_cast<TFormationBaseEntry*> (Source().Model().GraphEntry(MD_BASE_FORMATION));
  assert(pEntry);
  TFormationBaseEntry::TNodeSet stFormation = pEntry->EntryNodes();

  for(TFormationBaseEntry::TNodeSet::iterator it = stFormation.begin(); it != stFormation.end(); it++)
  {
    CFormationBase& formation = **it;
    // Add the formation to the combo
    int nIndex = m_cbFormation.AddString(formation.Name().toStdString().c_str());
    m_cbFormation.SetItemData(nIndex, (DWORD_PTR)(&formation));
    
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

template<class T>
void CAttriFormationElementSet<T>::OnPaint() 
{
  if(GetDlgItem(IDC_ST_COLOR))
  {
    CPaintDC dc(this); // device context for painting
    FillFrame(IDC_ST_COLOR, Copy().Color(), dc);
  }

  CAttributesTemplate<T>::OnPaint();
}


#endif // !defined(AFX_ATTRIFORMATIONELEMENTSET_H__8298992F_57D7_4E5E_A561_D5E204717869__INCLUDED_)
