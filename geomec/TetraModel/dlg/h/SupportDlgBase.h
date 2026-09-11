// SupportDlgBase.h: interface for the CSupportDlgBase class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SUPPORTDLGBASE_H__B132AAC4_4A95_47F2_9DCD_A552942E08DB__INCLUDED_)
#define AFX_SUPPORTDLGBASE_H__B132AAC4_4A95_47F2_9DCD_A552942E08DB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class C3DSupportNode;
class CModelBase;
class CSupportDlgBase : public CDialog  
{
public:
  CSupportDlgBase(const C3DSupportNode &node, unsigned int nID, CWnd *pParent = NULL);
  virtual ~CSupportDlgBase();

  BOOL ApplyToAll() { return m_bApplyToAll; }

protected:
  void OnSelchangeStage(CComboBox *pCombo);
  BOOL OnInitDialog();
  virtual void DoDataExchange(CDataExchange* pDX);
  void FillDepletionStageCombo(CComboBox *pCombo);
  const CModelBase &Model();
  enum IQuantityDouble::UNIT Unit();
  void OnRadioButton();
  virtual void OnOK(unsigned int nPromptId);
  const C3DSupportNode &Node() { return m_Node; }

  std::vector<const CDepletionStage*> m_vcDepletionStages;
  int m_nDeplStage;

  virtual void FillDepletionStageCombo() = 0;
  virtual void SetRadioButton() = 0;
  virtual void GetRadioButton() = 0;
  virtual void UpdateControls() = 0;
  virtual void FillUnitLabels() = 0;
  virtual void EmptyEditBoxes() = 0;

private:
  const C3DSupportNode &m_Node;
  BOOL m_bApplyToAll;

  void FillDepletionStageVector();
};

#endif // !defined(AFX_SUPPORTDLGBASE_H__B132AAC4_4A95_47F2_9DCD_A552942E08DB__INCLUDED_)
