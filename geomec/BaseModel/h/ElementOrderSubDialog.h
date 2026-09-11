#ifndef _ElementOrderSubDialog_h_
#define _ElementOrderSubDialog_h_

class CFormationBase;

#include "ElementOrder.h"

class CElementOrderSubDialog
{
  public:
    CElementOrderSubDialog(CDialog* dialog, CFormationBase& formationBase);

    void doDataExchange(CDataExchange* dx);
    void onBnClickedInheritFromModel();
    BOOL onInitDialog();
    void onOK();

  private:
    CElementOrderSubDialog(const CElementOrderSubDialog& rhs);
    CElementOrderSubDialog& operator = (CElementOrderSubDialog rhs);

    static int elementOrder2RadioButtonIndex(
      CElementOrder::TElementOrder elementOrder);

    CDialog* m_dialog;
    CFormationBase& m_formationBase;
    BOOL m_inheritFromModel;
    int m_elementOrder;
};

#endif  // _ElementOrderSubDialog_h_
