#include "stdafx.h"

#include "ElementOrderSubDialog.h"
#include "FormationBase.h"

CElementOrderSubDialog::CElementOrderSubDialog(CDialog* dialog,
  CFormationBase& formationBase)
: m_dialog(dialog)
, m_formationBase(formationBase)
, m_inheritFromModel(
    formationBase.inheritElementOrderFromModel() ? TRUE : FALSE)
, m_elementOrder(elementOrder2RadioButtonIndex(formationBase.ElementOrder()))
{
}

void CElementOrderSubDialog::doDataExchange(CDataExchange* dx)
{
  DDX_Check(dx, IDC_INHERITFROMMODEL, m_inheritFromModel);
  DDX_Radio(dx, IDC_ORDERLINEAR, m_elementOrder);
}

namespace
{

// <key, mapped value> == <formation ID (name, index), element order>

typedef std::map <QString, int> TFormation2ElementOrder;

static TFormation2ElementOrder previousElementOrder;

const QString FORMATION_ID_FORMAT = QObject::tr("%1%2");

QString createFormationID(const CFormationBase& formationBase)
{
  return FORMATION_ID_FORMAT.
    arg(formationBase.Name()).arg(formationBase.Index());
}

void updateElementOrderRadioButtons(const CDialog* dialog, BOOL enable)
{
  dialog->GetDlgItem(IDC_ORDERLINEAR)->EnableWindow(enable);
  dialog->GetDlgItem(IDC_ORDERQUADRATIC)->EnableWindow(enable);
  dialog->GetDlgItem(IDC_ORDER_INIQUAD)->EnableWindow(enable);
}

} // anonymous namespace

void CElementOrderSubDialog::onBnClickedInheritFromModel()
{
  TFormation2ElementOrder::iterator elementOrder;
  QString formationID = createFormationID(m_formationBase);

  if ((elementOrder = previousElementOrder.find(formationID)) ==
    previousElementOrder.end())
  {
    std::pair <TFormation2ElementOrder::iterator, bool> result =
      previousElementOrder.insert(std::make_pair(formationID, m_elementOrder));

    assert(result.second);

    elementOrder = result.first;
  }

  m_dialog->UpdateData(TRUE);

  updateElementOrderRadioButtons(m_dialog, !m_inheritFromModel);

  if (m_inheritFromModel)
  {
    (*elementOrder).second = m_elementOrder;
    m_elementOrder = elementOrder2RadioButtonIndex(
      dynamic_cast <const CModelBase&> (
        m_formationBase.Model()).ElementOrder());
  }
  else
  {
    m_elementOrder = (*elementOrder).second;
  }

  m_dialog->UpdateData(FALSE);
}

BOOL CElementOrderSubDialog::onInitDialog()
{
  updateElementOrderRadioButtons(m_dialog, !m_inheritFromModel);

  m_dialog->UpdateData(FALSE);

  return TRUE;
}

void CElementOrderSubDialog::onOK()
{
  m_dialog->UpdateData(TRUE);

  m_formationBase.
    inheritElementOrderFromModel(m_inheritFromModel ? true : false);

  switch (m_elementOrder)
  {
    case 0:
      m_formationBase.ElementOrder(CElementOrder::EO_LINEAR);
      break;
    case 1:
      m_formationBase.ElementOrder(CElementOrder::EO_QUADRATIC);
      break;
    case 2:
      m_formationBase.ElementOrder(CElementOrder::EO_INIQUAD);
      break;
    default:
      assert(false);
  }
}

// private

// static

int CElementOrderSubDialog::elementOrder2RadioButtonIndex(
  CElementOrder::TElementOrder elementOrder)
{
  int radioButtonIndex = 0;

  switch (elementOrder)
  {
    case CElementOrder::EO_LINEAR:
      radioButtonIndex = 0;
      break;
    case CElementOrder::EO_QUADRATIC:
      radioButtonIndex = 1;
      break;
    case CElementOrder::EO_INIQUAD:
      radioButtonIndex = 2;
      break;
    default:
      assert(false);
  }

  return radioButtonIndex;
}
