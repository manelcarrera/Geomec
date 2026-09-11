// SupportDlgBase.cpp: implementation of the CSupportDlgBase class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "geomec.h"

#include "ModelBase.h"
#include "DepletionStage.h"
#include "SupportDlgBase.h"
#include "3DSupportNode.h"
#include "GlobalMessage.h"

#ifdef _DEBUG
#ifdef _MSC_VER
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#endif  // _MSC_VER
//#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSupportDlgBase::CSupportDlgBase(const C3DSupportNode &node, unsigned int nID, CWnd *pParent /*= NULL*/)
:	CDialog(nID, pParent),
  m_Node(node),
  m_nDeplStage(0),
  m_bApplyToAll(FALSE)
{

}

CSupportDlgBase::~CSupportDlgBase()
{

}

void CSupportDlgBase::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);

  DDX_Check(pDX, IDC_APPLYTOALL, m_bApplyToAll);
}

void CSupportDlgBase::FillDepletionStageCombo(CComboBox *pCombo)
{
  pCombo->ResetContent();
  const CDepletionStage *pStage = &Model().InitialDepletionStage();
  assert(pStage != 0);
  assert(!pStage->Last());
  pStage = &pStage->Next();
  int nIndex = 0;
  while(pStage)
  {
    m_vcDepletionStages.push_back(pStage);
    pCombo->AddString(pStage->Name().toStdString().c_str());

    if(!pStage->Last()) pStage = &pStage->Next();
    else pStage = 0;
    nIndex++;
  }

  pCombo->SetCurSel(m_nDeplStage);
}

void CSupportDlgBase::FillDepletionStageVector()
{
  const CDepletionStage *pStage = &Model().InitialDepletionStage();
  assert(pStage != 0);
  assert(!pStage->Last());

  pStage = &pStage->Next();

  while(pStage)
  {
    m_vcDepletionStages.push_back(pStage);

    if(!pStage->Last()) pStage = &pStage->Next();
    else pStage = 0;
  }
}

enum IQuantityDouble::UNIT CSupportDlgBase::Unit()
{
  const CFemAppDoc *pDoc = GetGeomecDoc();
  assert(pDoc != 0);

  return pDoc->UnitNode().Unit();
}

BOOL CSupportDlgBase::OnInitDialog() 
{
  FillDepletionStageVector();

  CDialog::OnInitDialog();

  SetRadioButton();
  FillDepletionStageCombo();
  FillUnitLabels();

  UpdateControls();
  
  UpdateData(FALSE);

  return TRUE;
}

const CModelBase &CSupportDlgBase::Model()
{
  const CModelBase *pModel = dynamic_cast<const CModelBase *> (&m_Node.Model());
  assert(pModel != 0);
  return *pModel;
}

void CSupportDlgBase::OnRadioButton()
{
  if(!UpdateData(TRUE))
  {
    SetRadioButton();
    return;
  }

  GetRadioButton();
  UpdateControls();
  UpdateData(FALSE);
}

void CSupportDlgBase::OnSelchangeStage(CComboBox *pCombo) 
{
  if(!UpdateData(TRUE))
  {
    pCombo->SetCurSel(m_nDeplStage);
    return;
  }
  m_nDeplStage = pCombo->GetCurSel();
  SetRadioButton();
  UpdateControls();
  UpdateData(FALSE);
}

void CSupportDlgBase::OnOK(unsigned int nPromptId)
{
  UpdateData(TRUE);
  if(ApplyToAll())
  {
    if(_m()->msg(nPromptId, MB_YESNO) == IDNO) return;
  }
  
  CDialog::OnOK();
}
