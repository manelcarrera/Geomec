#ifndef SAMPLEDPOINTSETDLG_H
#define SAMPLEDPOINTSETDLG_H

#include <string>

class CPointSet;

#include "unitnode.h"

// CSampledPointSetDlg dialog

class CSampledPointSetDlg : public CDialog
{
  DECLARE_DYNAMIC(CSampledPointSetDlg)

public:
  CSampledPointSetDlg
  (const CPointSet &pointSet
  , const CUnitNode::TUnitType unit
  , CWnd* pParent = NULL
  ); 
  virtual ~CSampledPointSetDlg();
  double getSizeNorth() const;
  double getSizeEast() const;
  double getSizeDepth() const;
  CString getName() const;


// Dialog Data
  enum { IDD = IDD_SAMPLEDPOINTSET };

private:
  const CPointSet &m_pointSet;
  double m_dSizeNorth;
  double m_dSizeEast;
  double m_dSizeDepth;
  std::string m_sName;
  const CUnitNode::TUnitType m_unit;
  void EnableOK();
  double GetValue(int fieldId);

protected:
  BOOL OnInitDialog();
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
  //{{AFX_MSG(CSampledPointSetDlg)
  afx_msg void OnNValueChange();
  afx_msg void OnEValueChange();
  afx_msg void OnDValueChange();
  afx_msg void OnNameChange();
  //}}AFX_MSG
  
  DECLARE_MESSAGE_MAP()
};
#endif // SAMPLEDPOINTSETDLG_H
