// D:\users\tnmx\trunk\geomec3\SRC\RichhInputSummaryDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Geomec.h"
#include "RichhInputSummaryDlg.h"
#include "ModelBase.h"
#include "TetraModel.h"
#include "MeshBase.h"
#include "GlobalPressure.h"
#include "GlobalTemperature.h"
#include "GlobalInitialStressNode.h"
#include "Pressure.h"
#include "Temperature.h"
#include "VolumetricStrainLoad.h"
#include "IMaterial.h"
//#include "MatLib.h"
#include "MaterialEntry.h"
#include "MaterialServer.h"
//#include "MatUnit.h"
#include "TnoFileDialog.h"
#include "GlobalMessage.h"


// CRichhInputSummaryDlg dialog

IMPLEMENT_DYNAMIC(CRichhInputSummaryDlg, CDialog)

CRichhInputSummaryDlg::CRichhInputSummaryDlg(const CModelBase &model, CWnd* pParent /*=NULL*/)
	: CDialog(CRichhInputSummaryDlg::IDD, pParent), m_Model(model), m_Unit(GetGeomecDoc()->UnitNode().Unit())
  , m_dumpModel(m_Model)
{
  const CDepletionStage *pStage = &model.InitialDepletionStage();
	while(pStage)
	{
    m_vcStages.push_back(pStage);
    if(pStage->Last())
			pStage = 0;
		else
			pStage = &pStage->Next();
  }

  TFormationBaseEntry& entry = (TFormationBaseEntry&)*model.GraphEntry(MD_BASE_FORMATION);
	TFormationBaseEntry::TNodeSet stNode = entry.EntryNodes();
	for(TFormationBaseEntry::TNodeSet::const_iterator it = stNode.begin(); it != stNode.end(); it++)
	{
    m_vcFormations.push_back(*it);
  }
}

CRichhInputSummaryDlg::~CRichhInputSummaryDlg()
{
}

BOOL CRichhInputSummaryDlg::OnInitDialog()
{
  CDialog::OnInitDialog();

  m_stExcludedMatParameters.insert(CString("Bulk Stiffness"));
  m_stExcludedMatParameters.insert(CString("Shear Modulus"));
  m_stExcludedMatParameters.insert(CString("Dynamic Uniaxial Stiffness"));
  m_stExcludedMatParameters.insert(CString("Dynamic Shear Stiffness"));
  m_stExcludedMatParameters.insert(CString("Uniaxial Elastic Compressibility"));
  m_stExcludedMatParameters.insert(CString("Vp"));
  m_stExcludedMatParameters.insert(CString("Vs"));

  CString modelName;

  GetGeomecDoc()->ModelName(modelName);

  m_dumpModel.dump((LPCSTR) modelName, (LPCSTR) GetGeomecDoc()->GetPathName());

  std::vector <CDumpModel::TLineType> lineTypes = m_dumpModel.getLineTypes();
  std::vector <QString> lines = m_dumpModel.getLines();
  std::vector <CDumpModel::TLineType> ::const_iterator lineType =
    lineTypes.begin();
  std::vector <QString> ::const_iterator line = lines.begin();

  while ((lineType != lineTypes.end()) && (line != lines.end()))
  {
    switch (*lineType)
    {
      case CDumpModel::text:
        AddText(line->toStdString().c_str());
        break;
      case CDumpModel::newLine:
        NewLine();
        break;
      case CDumpModel::boldText:
        AddBoldText(line->toStdString().c_str());
        break;
      case CDumpModel::boldUnderlinedText:
        AddBoldUnderlinedText(line->toStdString().c_str());
        break;
      case CDumpModel::redText:
        AddRedText(line->toStdString().c_str());
        break;
      default:
        assert(false);
    }

    ++line;
    ++lineType;
  }

  assert(line == lines.end());
  assert(lineType == lineTypes.end());

  return TRUE;
}

void CRichhInputSummaryDlg::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
  DDX_Control(pDX, IDC_SUMMARY_RICHEDIT, m_ctrlSummary);
}

void CRichhInputSummaryDlg::AddText(const CString &text)
{
  int nBegin;
  nBegin = m_ctrlSummary.GetTextLength();
  m_ctrlSummary.SetSel(nBegin, nBegin);   // Select last character
  m_ctrlSummary.ReplaceSel(text);          // Append, move cursor to end of text
  m_ctrlSummary.SetSel(-1,0);             // Remove Black selection bars
  nBegin = m_ctrlSummary.GetTextLength(); // Get New Length
  m_ctrlSummary.SetSel(nBegin,nBegin);    // Cursor to End of new text
}

void CRichhInputSummaryDlg::NewLine(int nNewLines)
{
  for(int i = 0; i < nNewLines; ++i)
  {
    AddText(_T("\n"));
  }
}

void CRichhInputSummaryDlg::AddBoldText(const CString &text) 
{
  CHARFORMAT Cfm;
  CHARFORMAT defaultCfm;

	m_ctrlSummary.GetSelectionCharFormat(Cfm);
  defaultCfm = Cfm;
  Cfm.cbSize = sizeof(CHARFORMAT);
	Cfm.dwMask = CFM_BOLD;
	Cfm.dwEffects ^= CFE_BOLD; 
	m_ctrlSummary.SetSelectionCharFormat(Cfm);
  AddText(text);
  m_ctrlSummary.SetSelectionCharFormat(defaultCfm);
}

void CRichhInputSummaryDlg::AddBoldUnderlinedText(const CString &text) 
{
  CHARFORMAT Cfm;
  CHARFORMAT defaultCfm;

	m_ctrlSummary.GetSelectionCharFormat(Cfm);
  defaultCfm = Cfm;
  Cfm.cbSize = sizeof(CHARFORMAT);
	Cfm.dwMask = CFM_BOLD|CFM_UNDERLINE;
	Cfm.dwEffects ^= CFE_BOLD|CFE_UNDERLINE; 
	m_ctrlSummary.SetSelectionCharFormat(Cfm);
  AddText(text);
  m_ctrlSummary.SetSelectionCharFormat(defaultCfm);
}

void CRichhInputSummaryDlg::AddRedText(const CString &text)
{
  
}

BEGIN_MESSAGE_MAP(CRichhInputSummaryDlg, CDialog)
 
  ON_BN_CLICKED(IDC_SUMMARY_SAVE_BUTTON, &CRichhInputSummaryDlg::OnBnClickedSummarySaveButton)
END_MESSAGE_MAP()


// CRichhInputSummaryDlg message handlers

void CRichhInputSummaryDlg::OnBnClickedSummarySaveButton()
{
  CString sDefaultName;
  GetGeomecDoc()->ModelName(sDefaultName);
  sDefaultName = sDefaultName + "_Summary";
  CString	sFilter = "Rich Text Format Files (*.rtf)|*.rtf|All Files (*.*)|*.*|";

  CTnoFileDialog dialog(FALSE, "rtf", sDefaultName, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, sFilter);
  if(dialog.DoModal() == IDOK)
  {
    EDITSTREAM es;
    try
    {
      CFile fo(dialog.GetPathName(), CFile::modeCreate|CFile::modeWrite);
      es.dwCookie = (DWORD_PTR)&fo;
      es.pfnCallback = (EDITSTREAMCALLBACK)EditStreamCallback;
      
      m_ctrlSummary.StreamOut(SF_RTF, es);
    }
    catch(...) 
    {
      _m()->msg("Unable to write file", MB_OK|MB_ICONEXCLAMATION);
    }
  }
}

DWORD CALLBACK CRichhInputSummaryDlg::EditStreamCallback(DWORD_PTR dwCookie, LPBYTE pbBuff, LONG cb, LONG *pcb)
{
  CFile* pFile = (CFile*)dwCookie;

  pFile->Write(pbBuff,cb);
  *pcb = cb;
  return 0;
}
