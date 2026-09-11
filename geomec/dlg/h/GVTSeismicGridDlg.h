#if !defined(GVTSEISMICGRIDDLG_H_)
#define GVTSEISMICGRIDDLG_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// CGVTSeismicGridDlg dialog
class CGVTSeismicGridDefinition;

class CGVTSeismicGridDlg : public CDialog {
  DECLARE_DYNAMIC(CGVTSeismicGridDlg)

  const CModelBase &m_Model;
  CString m_Name;
  CString m_FilePath;
  double m_Northing;
  double m_Easting;
  double m_Azimuth;
  double m_Depth;
  double m_TrackSpacing;
  double m_BinSpacing;
  int m_DeltaBinNum;
  int m_DeltaTrackNum;
  int m_FirstBin;
  int m_FirstTrack;
  int m_NumBin;
  int m_NumTrack;

  CGVTSeismicGridDefinition &m_SeismicGridDefinition;

public:
  CGVTSeismicGridDlg(CGVTSeismicGridDefinition &griddefinition, const CModelBase &model,
                     CWnd *pParent = NULL); // standard constructor
  virtual ~CGVTSeismicGridDlg();

  // Dialog Data
  enum { IDD = IDD_GVT_SEISMIC_GRID };

protected:
  virtual void DoDataExchange(CDataExchange *pDX); // DDX/DDV support
  virtual BOOL OnInitDialog();
  virtual void OnOK();

  DECLARE_MESSAGE_MAP()
public:
  afx_msg void OnBnClickedButtonGvtbrowse();

  afx_msg void OnEnChangeEditGvtname();
  afx_msg void OnEnUpdateEditGvtname();
};
#endif