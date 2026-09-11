#include "stdafx.h"
#include "Geomec.h"
#include "AttriThinLayerUpscalingTargetPointsets.h"
#include "Upscaling.h"

CAttriUpscalingTargetPointsets::CAttriUpscalingTargetPointsets(CUpscalingPointsetRangesBranch& branch, CWnd* pParent /*=NULL*/)
: CDialog(IDD_ATTRI_THINLAYERUPSCALING_TARGET_POINTSETS, pParent),
  m_branch(branch)
{
  m_nAll = (branch.UseAllPointsets() ? 1 : 0);
}

CAttriUpscalingTargetPointsets::~CAttriUpscalingTargetPointsets()
{
}

void CAttriUpscalingTargetPointsets::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
  DDX_Radio(pDX, IDC_RADIO_ANY, m_nAll);
}

void CAttriUpscalingTargetPointsets::OnOK()
{
  UpdateData(TRUE);
  m_branch.UseAllPointsets(m_nAll != 0);
  CDialog::OnOK();
}
