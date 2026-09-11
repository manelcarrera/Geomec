#pragma once

// CPetrelFileUnitDialog dialog

class CPetrelFileUnitDialog : public CDialog
{
public:
  CPetrelFileUnitDialog(const CString& petrelFile, CQuantity::UNIT lateralUnit,
  CQuantity::UNIT depthUnit, CWnd* pParent = NULL);
  virtual ~CPetrelFileUnitDialog();

  CQuantity::UNIT getLateralUnit() const;
  CQuantity::UNIT getDepthUnit() const;

// Dialog Data
  enum { IDD = IDD_PETREL_FILE_UNIT };

protected:
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

  DECLARE_MESSAGE_MAP()

private:
  int m_LateralUnit;
  int m_DepthUnit;
  CString m_PetrelFile;
};
