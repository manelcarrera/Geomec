// GVTSeismicGridDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Geomec.h"
#include "GVTSettings.h"
#include "GVTSeismicGridDefinition.h"
#include "GVTSeismicGridDlg.h"
#include "TnoFileDialog.h"
#include "LengthQuantity.h"
#include "ModelBase.h"
#include "MeshBase.h"
#include "GlobalMessage.h"

// CGVTSeismicGridDlg dialog

IMPLEMENT_DYNAMIC(CGVTSeismicGridDlg, CDialog)

CGVTSeismicGridDlg::CGVTSeismicGridDlg(CGVTSeismicGridDefinition &griddefinition, const CModelBase &model,CWnd* pParent /*=NULL*/)
  : CDialog(CGVTSeismicGridDlg::IDD, pParent),
  m_Model(model),
  m_SeismicGridDefinition(griddefinition),
  m_Name(griddefinition.Name().toStdString().c_str()),
  m_FilePath(griddefinition.KeyFile().toStdString().c_str()),
  m_Northing(griddefinition.CornerNorthing()),
  m_Easting(griddefinition.CornerEasting()),
  m_Depth(griddefinition.Depth()?griddefinition.Depth():model.Mesh().Max().Z()),
  m_Azimuth(griddefinition.Azimuth()),
  m_TrackSpacing(griddefinition.TrackSpacing()),
  m_BinSpacing(griddefinition.BinSpacing()),
  m_DeltaBinNum(griddefinition.DeltaBin()),
  m_DeltaTrackNum(griddefinition.DeltaTrack()),
  m_FirstBin(griddefinition.FirstBin()),
  m_FirstTrack(griddefinition.FirstTrack()),
  m_NumBin(griddefinition.NumBin()),
  m_NumTrack(griddefinition.NumTrack())
{
}

BOOL CGVTSeismicGridDlg::OnInitDialog()
{
  CLengthQuantity lq;
  CUnitNode::TUnitType unit = GetGeomecDoc()->UnitNode().Unit();
  CString strLengthUnit = lq.UnitName(unit).c_str();

  GetDlgItem(IDC_STATIC_GVTNORTHUNIT)->SetWindowText(strLengthUnit);
  GetDlgItem(IDC_STATIC_GVTEASTUNIT)->SetWindowText(strLengthUnit);
  GetDlgItem(IDC_STATIC_GVTDEPTHUNIT)->SetWindowText(strLengthUnit);
  GetDlgItem(IDC_STATIC_GVTTRACKUNIT)->SetWindowText(strLengthUnit);
  GetDlgItem(IDC_STATIC_GVTBINUNIT)->SetWindowText(strLengthUnit);

  UpdateData(FALSE);
  return TRUE;
}

void CGVTSeismicGridDlg::OnOK()
{
  UpdateData();

  m_SeismicGridDefinition.KeyFile((LPCSTR) m_FilePath);
  m_SeismicGridDefinition.Azimuth(m_Azimuth);
  m_SeismicGridDefinition.Name((LPCSTR) m_Name);
  m_SeismicGridDefinition.BinSpacing(m_BinSpacing);
  m_SeismicGridDefinition.CornerEasting(m_Easting);
  m_SeismicGridDefinition.CornerNorthing(m_Northing);
  m_SeismicGridDefinition.Depth(m_Depth);
  m_SeismicGridDefinition.TrackSpacing(m_TrackSpacing);
  m_SeismicGridDefinition.DeltaBin(m_DeltaBinNum);
  m_SeismicGridDefinition.DeltaTrack(m_DeltaTrackNum);
  m_SeismicGridDefinition.FirstBin(m_FirstBin);
  m_SeismicGridDefinition.FirstTrack(m_FirstTrack);
  m_SeismicGridDefinition.NumBin(m_NumBin);
  m_SeismicGridDefinition.NumTrack(m_NumTrack);

  CDialog::OnOK();
}

CGVTSeismicGridDlg::~CGVTSeismicGridDlg()
{
}

void CGVTSeismicGridDlg::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);

  CLengthQuantity lq;
  CUnitNode::TUnitType unit = GetGeomecDoc()->UnitNode().Unit();

  double dBinSpacing;
  double dEasting;
  double dNorthing;
  double dDepth;
  double dTrackSpacing;

  if(!pDX->m_bSaveAndValidate)
  {
  dBinSpacing   = lq.Convert(m_BinSpacing,   unit, CLengthQuantity::SI_UNIT);
  dEasting      = lq.Convert(m_Easting,      unit, CLengthQuantity::SI_UNIT);
  dNorthing     = lq.Convert(m_Northing,     unit, CLengthQuantity::SI_UNIT);
  dDepth        = lq.Convert(m_Depth,        unit, CLengthQuantity::SI_UNIT);
  dTrackSpacing = lq.Convert(m_TrackSpacing, unit, CLengthQuantity::SI_UNIT);
  }

  DDX_Text(pDX, IDC_EDIT_GVTNAME,     m_Name);
  DDX_Text(pDX, IDC_EDIT_GVTFILEPATH, m_FilePath);

  DDX_Text(pDX, IDC_EDIT_GVTNORTHING, dNorthing);
  DDX_Text(pDX, IDC_EDIT_GVTEASTING,  dEasting);

  DDX_Text(pDX, IDC_EDIT_GVTAZIMUTH,  m_Azimuth);

  DDX_Text(pDX, IDC_EDIT_GVTDEPTH,    dDepth);

  DDX_Text(pDX, IDC_EDIT_GVTBIN,      dBinSpacing);
  DDX_Text(pDX, IDC_EDIT_GVTTRACK,    dTrackSpacing);

  DDX_Text(pDX, IDC_EDIT_FIRSTTRACK,  m_FirstTrack);
  DDX_Text(pDX, IDC_EDIT_FIRSTBIN,    m_FirstBin);

  DDX_Text(pDX, IDC_EDIT_NUMTRACK,    m_NumTrack);
  DDX_Text(pDX, IDC_EDIT_NUMBIN,      m_NumBin);

  if(pDX->m_bSaveAndValidate)
  {
  m_BinSpacing =   lq.Convert(dBinSpacing,   CLengthQuantity::SI_UNIT, unit);
  m_Easting =      lq.Convert(dEasting,      CLengthQuantity::SI_UNIT, unit);
  m_Northing =     lq.Convert(dNorthing,     CLengthQuantity::SI_UNIT, unit);
  m_Depth =        lq.Convert(dDepth,        CLengthQuantity::SI_UNIT, unit);
  m_TrackSpacing = lq.Convert(dTrackSpacing, CLengthQuantity::SI_UNIT, unit);
  }
}


BEGIN_MESSAGE_MAP(CGVTSeismicGridDlg, CDialog)
  ON_BN_CLICKED(IDC_BUTTON_GVTBROWSE, &CGVTSeismicGridDlg::OnBnClickedButtonGvtbrowse)
END_MESSAGE_MAP()


// CGVTSeismicGridDlg message handlers
void CGVTSeismicGridDlg::OnBnClickedButtonGvtbrowse()
{
  CString sFilter = "Shell File Format (*.vt)|*.vt|Key files (*.key)|*.key|All Files (*.*)|*.*||";
  CTnoFileDialog dlg(TRUE, 
          NULL, 
          NULL,
          NULL,
          sFilter);
  if(dlg.DoModal() == IDOK)
  {
  bool bNoData = static_cast<CButton *>(GetDlgItem(IDC_CHECK_VT_NODATA))->GetCheck() == BST_CHECKED;

  if (m_SeismicGridDefinition.Import(QString(dlg.GetPathName()), const_cast<CModelBase&>(m_Model), bNoData))
  {
      m_FilePath      = dlg.GetPathName();
      m_Northing      = m_SeismicGridDefinition.CornerNorthing();
      m_Easting       = m_SeismicGridDefinition.CornerEasting();
      m_Depth         = m_SeismicGridDefinition.Depth(); // ?griddefinition.Depth():model.Mesh().Max().Z()),
      m_Azimuth       = m_SeismicGridDefinition.Azimuth();
      m_TrackSpacing  = m_SeismicGridDefinition.TrackSpacing();
      m_BinSpacing    = m_SeismicGridDefinition.BinSpacing();
      m_DeltaBinNum   = m_SeismicGridDefinition.DeltaBin();
      m_DeltaTrackNum = m_SeismicGridDefinition.DeltaTrack();
      m_FirstBin      = m_SeismicGridDefinition.FirstBin();
      m_FirstTrack    = m_SeismicGridDefinition.FirstTrack();
      m_NumBin        = m_SeismicGridDefinition.NumBin();
      m_NumTrack      = m_SeismicGridDefinition.NumTrack();

      UpdateData(FALSE);
  }
  }
  
}


