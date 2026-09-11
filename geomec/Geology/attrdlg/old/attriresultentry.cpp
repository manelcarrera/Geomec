// attriresultentry.cpp : implementation file
//

#include "stdafx.h"
#ifdef _MSC_VER
#pragma warning ( disable : 4786 )
#endif  // _MSC_VER
#include "geomec.h"
#include "attriresultentry.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#ifdef _MSC_VER#undef THIS_FILE
static char THIS_FILE[] = __FILE__;#endif  // _MSC_VER
#endif


/////////////////////////////////////////////////////////////////////////////
// CAttriResultEntry dialog


CAttriResultEntry::CAttriResultEntry(CResultEntry &entry, CWnd* pParent /*=NULL*/)
  : CDialog(CAttriResultEntry::IDD, pParent), m_entry(entry)
{
  //{{AFX_DATA_INIT(CAttriResultEntry)
  //}}AFX_DATA_INIT
}


void CAttriResultEntry::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
  int nAverage = -1;
  if(!pDX->m_bSaveAndValidate)
  {
    switch(m_entry.WeightingType())
    {
    case IValueComposite::NO_WEIGHTING:
      nAverage = 0;
      break;
    case IValueComposite::VOLUME:
      nAverage = 1;
      break;
    case IValueComposite::INVERSE_VOLUME:
      nAverage = 2;
      break;
    }
  }

  //{{AFX_DATA_MAP(CAttriResultEntry)
  DDX_Radio(pDX, IDC_RB_UNWEIGTHED, nAverage);
  //}}AFX_DATA_MAP

  if(pDX->m_bSaveAndValidate)
  {
    switch(nAverage)
    {
    case 0:
      m_entry.WeightingType(IValueComposite::NO_WEIGHTING);
      break;
    case 1:
      m_entry.WeightingType(IValueComposite::VOLUME);			
      break;
    case 2:
      m_entry.WeightingType(IValueComposite::INVERSE_VOLUME);			
      break;
    default:
      ASSERT(FALSE);
    }
  }
}


BEGIN_MESSAGE_MAP(CAttriResultEntry, CDialog)
  //{{AFX_MSG_MAP(CAttriResultEntry)
    // NOTE: the ClassWizard will add message map macros here
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAttriResultEntry message handlers
