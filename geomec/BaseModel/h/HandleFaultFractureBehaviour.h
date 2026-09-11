#ifndef _HandleFaultFractureBehaviour_h_
#define _HandleFaultFractureBehaviour_h_

#include "HorizonBase.h"

template <typename C> class CHandleFaultFractureBehaviour {
public:
  CHandleFaultFractureBehaviour(CDialog *dialog);

  bool onCommand(C &copy, WPARAM wParam, LPARAM lParam);

  void onStick(C &copy);
  void onSlip(C &copy);
  void onUserDefined(C &copy);
  void onFracture(C &copy);

  void enableSlipButtons(bool enable);

  int getSlipType(const C &copy) const;
  void setSlipType(CDataExchange *pDX, int &slipType);

private:
  CHandleFaultFractureBehaviour(const CHandleFaultFractureBehaviour &rhs);
  CHandleFaultFractureBehaviour &operator=(const CHandleFaultFractureBehaviour &rhs);

  CDialog *m_dialog;
};

template <typename C>
CHandleFaultFractureBehaviour<C>::CHandleFaultFractureBehaviour(CDialog *dialog) : m_dialog(dialog) {}

template <typename C> bool CHandleFaultFractureBehaviour<C>::onCommand(C &copy, WPARAM wParam, LPARAM lParam) {
  switch (wParam) {
  case IDC_RADIO_STICK:
    onStick(copy);
    return true;
  case IDC_RADIO_SLIP:
    onSlip(copy);
    return true;
  case IDC_RADIO_USER:
    onUserDefined(copy);
    return true;
  case IDC_RADIO_FRACTURE:
    onFracture(copy);
    return true;
  default:
    break;
  }

  return false;
}

template <typename C> void CHandleFaultFractureBehaviour<C>::onStick(C &copy) { copy.SlipType(CHorizonBase::STICK); }

template <typename C> void CHandleFaultFractureBehaviour<C>::onSlip(C &copy) { copy.SlipType(CHorizonBase::SLIP); }

template <typename C> void CHandleFaultFractureBehaviour<C>::onUserDefined(C &copy) {
  copy.SlipType(CHorizonBase::USER);
}

template <typename C> void CHandleFaultFractureBehaviour<C>::onFracture(C &copy) {
  copy.SlipType(CHorizonBase::FRACTURE);
}

template <typename C> void CHandleFaultFractureBehaviour<C>::enableSlipButtons(bool enable) {
  m_dialog->GetDlgItem(IDC_RADIO_SLIP)->EnableWindow(enable);
  m_dialog->GetDlgItem(IDC_RADIO_STICK)->EnableWindow(enable);
  m_dialog->GetDlgItem(IDC_RADIO_USER)->EnableWindow(enable);
  m_dialog->GetDlgItem(IDC_RADIO_FRACTURE)->EnableWindow(enable);
}

template <typename C> int CHandleFaultFractureBehaviour<C>::getSlipType(const C &copy) const {
  int slipType = 0;

  switch (copy.SlipType()) {
  case CHorizonBase::SLIP:
    slipType = 0;
    break;
  case CHorizonBase::STICK:
    slipType = 1;
    break;
  case CHorizonBase::USER:
    slipType = 2;
    break;
  case CHorizonBase::FRACTURE:
    slipType = 3;
    break;
  default:
    assert(false);
  }

  return slipType;
}

template <typename C> void CHandleFaultFractureBehaviour<C>::setSlipType(CDataExchange *pDX, int &slipType) {
  DDX_Radio(pDX, IDC_RADIO_SLIP, slipType);
}

#endif // _HandleFaultFractureBehaviour_h_
