#if !defined(AFX_ATTRIHORIZON__INCLUDED_)
#define AFX_ATTRIHORIZON__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "AttributesTemplate.h"
#include "BaseEntryTypes.h"
#include "BranchState.h"
#include "FaultFracture.h"
#include "GlobalUnitNode.h"
#include "HandleDefaultFaultParameters.h"
#include "HandleFaultFractureBehaviour.h"
#include "ModelBase.h"

class CFormationBase;

template <class T> class CAttriHorizon : public CAttributesTemplate<T> {
  bool m_bSlipSwitch;
  bool m_bInitialUpdate;
  CHandleFaultFractureBehaviour<T> m_handleFaultFractureBehaviour;

public:
  CAttriHorizon(unsigned int uDialogTemplate, T &horizon, bool bSlipSwitch, CWnd *pParent = NULL);

  virtual BOOL OnInitDialog();
  virtual void DoDataExchange(CDataExchange *pDX); // DDX/DDV support
  virtual BOOL OnWndMsg(unsigned int message, WPARAM wParam, LPARAM lParam, LRESULT *pResult);
  virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);

private:
  void OnChangeColor();
  void OnPaint();
  void OnSlipLayer();
};

/*!
  Constructor
  \param uDialogTemplate Template resource ID
  \param horizon         Horizon or fault
  \param bSlipSwtich     TRUE the user can switch with IDC_CK_SLIP from fault to horzion
  \param pParent         Parent window
*/
template <class T>
CAttriHorizon<T>::CAttriHorizon(unsigned int uDialogTemplate, T &horizon, bool bSlipSwitch, CWnd *pParent)
    : CAttributesTemplate<T>(uDialogTemplate, horizon, pParent), m_bSlipSwitch(bSlipSwitch), m_bInitialUpdate(true),
      m_handleFaultFractureBehaviour(this) {}

template <class T> BOOL CAttriHorizon<T>::OnInitDialog() {
  CAttributesTemplate<T>::OnInitDialog();

  const CHorizonBase *pSource = dynamic_cast<const CHorizonBase *>(&Copy().Source());
  assert(pSource != 0);
  if (m_bSlipSwitch) {
    // There is a slip on/off button...
    if (!pSource->CanSlip()) {
      assert(!Copy().Slip());
      GetDlgItem(IDC_CK_SLIP)->EnableWindow(FALSE);
    }
  }

  if (pSource->DoubleSidedFault()) {
    GetDlgItem(IDC_DOUBLE_SIDED_FAULT)->ShowWindow(TRUE);
  }

  if ((static_cast<const CModelBase &>(Copy().Model())).BranchState().IsBranch()) {
    CWnd *pWnd = GetDlgItem(IDC_CK_SLIP);
    if (pWnd)
      pWnd->EnableWindow(FALSE);
  }

  return TRUE; // return TRUE unless you set the focus to a control
               // EXCEPTION: OCX Property Pages should return FALSE
}

template <class T> void CAttriHorizon<T>::DoDataExchange(CDataExchange *pDX) {
  // Variables ....
  CString strName;
  int nSlipType;
  BOOL bSlip;
  bool bBranch = (static_cast<const CModelBase &>(Copy().Model())).BranchState().IsBranch();

  if (!pDX->m_bSaveAndValidate) {
    strName = Copy().Name().toStdString().c_str();

    if (Copy().Slip() || m_bSlipSwitch) {
      // Initialise the slip section
      bSlip = Copy().Slip();
      nSlipType = m_handleFaultFractureBehaviour.getSlipType(Copy());
      m_handleFaultFractureBehaviour.enableSlipButtons(bSlip && !bBranch);
    }
  } else
    m_bInitialUpdate = true;

  CAttributesTemplate<T>::DoDataExchange(pDX);

  if (Copy().Slip() || m_bSlipSwitch) {
    if (m_bSlipSwitch)
      DDX_Check(pDX, IDC_CK_SLIP, bSlip); // Update one time ....
    // Do the check box if
    m_handleFaultFractureBehaviour.setSlipType(pDX, nSlipType);
  }

  if (m_bInitialUpdate) {
    DDX_Text(pDX, IDC_ED_NAME, strName); // Update one time ....
    m_bInitialUpdate = false;
  }

  if (pDX->m_bSaveAndValidate) {
    if (Copy().DoubleSidedFault()) {
      QString srcFault;
      if (Source().Name().endsWith("fault_plus"))
        srcFault = "fault_plus";
      else if (Source().Name().endsWith("fault_minus"))
        srcFault = "fault_minus";

      if (!srcFault.isEmpty() && strName.Right(srcFault.length()) != srcFault.toStdString().c_str())
        strName += srcFault.toStdString().c_str();
    }

    Copy().Name((LPCSTR)strName); // We always do it ...

    // Check consistency of the cohesion enz. when we have slip
    if (Copy().Slip() && !bBranch) {

      switch (nSlipType) {
      case 0:
        Copy().SlipType(T::SLIP);
        break;
      case 1:
        Copy().SlipType(T::STICK);
        break;
      case 2:
        Copy().SlipType(T::USER);
        break;
      case 3:
        Copy().SlipType(T::FRACTURE);
        break;
      default:
        assert(false);
        break;
      }
    }
  }

  if (Copy().Slip() && !bBranch) {
    CGlobalUnitNode globalUnitNode;

    CFaultFracture faultFracture(&Copy(), globalUnitNode.unitNode().Unit());

    CHandleDefaultFaultParameters handleDefaultFaultParameters(&faultFracture);
    handleDefaultFaultParameters.DoDataExchange(pDX);

    faultFracture.FinalizeInitD0();
  }
}

template <class T>
BOOL CAttriHorizon<T>::OnWndMsg(unsigned int message, WPARAM wParam, LPARAM lParam, LRESULT *pResult) {
  if (message == WM_PAINT) {
    OnPaint();
    return TRUE;
  }

  return CAttributesTemplate<T>::OnWndMsg(message, wParam, lParam, pResult);
}

template <class T> BOOL CAttriHorizon<T>::OnCommand(WPARAM wParam, LPARAM lParam) {
  unsigned int nID = wParam;

  switch (nID) {
  case IDC_ST_COLOR:
    OnChangeColor();
    return TRUE;
  case IDC_CK_SLIP:
    OnSlipLayer();
    return TRUE;
  default:
    bool succeeded = m_handleFaultFractureBehaviour.onCommand(Copy(), wParam, lParam);

    if (succeeded) {
      UpdateData(FALSE);
      return TRUE;
    }
  }

  return CAttributesTemplate<T>::OnCommand(wParam, lParam);
}

template <class T> void CAttriHorizon<T>::OnChangeColor() {
  Copy().Color(SelectColor(Copy().Color()));
  Invalidate();
}

template <class T> void CAttriHorizon<T>::OnPaint() {
  if (GetDlgItem(IDC_ST_COLOR)) {
    CPaintDC dc(this); // device context for painting
    FillFrame(IDC_ST_COLOR, Copy().Color(), dc);
  }

  CAttributesTemplate<T>::OnPaint();
}

template <class T> void CAttriHorizon<T>::OnSlipLayer() {
  // Handle update here locally
  Copy().Slip(!Copy().Slip());
  m_handleFaultFractureBehaviour.enableSlipButtons(Copy().Slip());
}

#endif // !defined(AFX_ATTRIHORIZON__INCLUDED_)
