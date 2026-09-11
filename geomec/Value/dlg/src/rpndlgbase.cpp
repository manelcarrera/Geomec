// RPNDlg.cpp : implementation file
//

#include "RpnDlgBase.h"
#include "ContextMenuInvoker.h"
#include "GlobalMessage.h"
#include "RpnConstant.h"
#include "RpnOperand.h"
#include "RpnTernaryOperation.h"
#include "ValueComponentProxyWrapper.h"
#include "rpnbinaryoperation.h"
#include "rpnconstantoperand.h"
#include "rpnstack.h"
#include "rpnunaryoperation.h"
#include "stdafx.h"

#ifdef _DEBUG
// #define new DEBUG_NEW
#ifdef _MSC_VER
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif // _MSC_VER
#endif

const unsigned int COMMAND_OFFSET = 300;

namespace rpn {
/////////////////////////////////////////////////////////////////////////////
// CRpnDlgBase::CValueProxyObject
CRpnDlgBase::CValueProxyListObject::CValueProxyListObject(CRpnDlgBase &dlg, CRpnOperand::IValueProxy &proxy)
    : IListObject(dlg.ListCtrl(), dlg.ListCtrl().GetItemCount(), 0), m_proxy(proxy), m_dlg(dlg) {
  Ctrl().SetItemData(Ctrl().GetItemCount() - 1, (DWORD_PTR)this);
  int width = Ctrl().GetStringWidth(Text().toStdString().c_str()) + 26;
  if (width > Ctrl().GetColumnWidth(0))
    Ctrl().SetColumnWidth(0, width);
}

CRpnDlgBase::CValueProxyListObject::CValueProxyListObject(CRpnDlgBase &dlg, CRpnOperand::IValueProxy &proxy,
                                                          int nListIndex)
    : IListObject(dlg.ListCtrl(), nListIndex, 0), m_proxy(proxy), m_dlg(dlg) {
  Ctrl().SetItemData(nListIndex, (DWORD_PTR)this);
  int width = Ctrl().GetStringWidth(Text().toStdString().c_str()) + 26;
  if (width > Ctrl().GetColumnWidth(0))
    Ctrl().SetColumnWidth(0, width);
}

QString CRpnDlgBase::CValueProxyListObject::Text() const { return m_proxy.TextTag(); }

unsigned int CRpnDlgBase::CValueProxyListObject::Icon() const { return m_proxy.IconId(); }

void CRpnDlgBase::CValueProxyListObject::OnDoubleClick() {
  new CRpnOperand(m_dlg.RpnStack(), m_proxy);
  m_dlg.UpdateData(FALSE);
}

void CRpnDlgBase::CValueProxyListObject::Delete() {
  m_proxy.Delete();
  m_dlg.UpdateStackDisplay();

  if (m_dlg.ValidRpnStack() == FALSE)
    m_dlg.OnBtClearAll();

  delete this;
}

bool CRpnDlgBase::CValueProxyListObject::CanRemove() const { return CanDelete() == TRUE; }

BOOL CRpnDlgBase::CValueProxyListObject::CanDelete() const { return m_proxy.CanDelete(); }

bool CRpnDlgBase::CValueProxyListObject::HasProperties() const { return m_proxy.HasProperties(); }

void CRpnDlgBase::CValueProxyListObject::Properties() {
  if (valueComponentProxyWrapper(m_proxy))
    m_dlg.UpdateStackDisplay();
}

/* NOT OBSOLETE */ void CRpnDlgBase::CValueProxyListObject::AppendContextMenu(CContextMenuInvoker &invoker) {
  typedef CSingleCommandTemplate<CRpnDlgBase::CValueProxyListObject> TCommand;
  invoker.AddCommand("&Remove", *(new TCommand(*this, &CRpnDlgBase::CValueProxyListObject::Delete,
                                               &CRpnDlgBase::CValueProxyListObject::CanRemove)));
  invoker.AddSeparator();
  invoker.AddCommand("&Properties", *(new TCommand(*this, &CRpnDlgBase::CValueProxyListObject::Properties,
                                                   &CRpnDlgBase::CValueProxyListObject::HasProperties)));
}

BOOL CRpnDlgBase::CValueProxyListObject::operator<(const ICtrlObjectBase &object) const {
  const CRpnDlgBase::CValueProxyListObject *pObject = dynamic_cast<const CRpnDlgBase::CValueProxyListObject *>(&object);
  if (pObject)
    return m_proxy.operator<(pObject->m_proxy);
  return IListObject::operator<(object);
}

/////////////////////////////////////////////////////////////////////////////
// CRpnDlgBase dialog

CRpnDlgBase::CRpnDlgBase(const CRpnStack &stack, unsigned int uDlgId, CWnd *pParent /*=NULL*/)
    : CDialog(uDlgId, pParent) {
  m_pStack = new CRpnStack(stack);

  // collect unit converters
  m_vcUnitConverters = CRpnUnitConverter::Collect();

  //{{AFX_DATA_INIT(CRpnDlgBase)
  // NOTE: the ClassWizard will add member initialization here
  //}}AFX_DATA_INIT
  // Note that LoadIcon does not require a subsequent DestroyIcon in Win32
}

CRpnDlgBase::~CRpnDlgBase() { delete m_pStack; }

CListCtrl &CRpnDlgBase::ListCtrl() { return m_lcOperand; }

BEGIN_MESSAGE_MAP(CRpnDlgBase, CDialog)
//{{AFX_MSG_MAP(CRpnDlgBase)
ON_WM_PAINT()
ON_WM_QUERYDRAGICON()
/*	ON_BN_CLICKED(IDC_BT_0, OnBtZero)
  ON_BN_CLICKED(IDC_BT_1, OnBtOne)
  ON_BN_CLICKED(IDC_BT_2, OnBtTwo)
  ON_BN_CLICKED(IDC_BT_3, OnBtThree)
  ON_BN_CLICKED(IDC_BT_4, OnBtFour)
  ON_BN_CLICKED(IDC_BT_5, OnBtFive)
  ON_BN_CLICKED(IDC_BT_6, OnBtSix)
  ON_BN_CLICKED(IDC_BT_7, OnBtSeven)
  ON_BN_CLICKED(IDC_BT_8, OnBtEight)
  ON_BN_CLICKED(IDC_BT_9, OnBtNine)
  ON_BN_CLICKED(IDC_BT_POINT, OnBtPoint)
  ON_BN_CLICKED(IDC_BT_ENTER, OnBtEnter)
  ON_BN_CLICKED(IDC_BT_ADD, OnBtAdd)
  ON_BN_CLICKED(IDC_BT_CLEAR, OnBtClear)
  ON_BN_CLICKED(IDC_BT_SUB, OnBtSub)
  ON_BN_CLICKED(IDC_BT_MULT, OnBtMult)
  ON_BN_CLICKED(IDC_BT_DIV, OnBtDiv)
  ON_BN_CLICKED(IDC_BT_SIN, OnBtSin)
  ON_BN_CLICKED(IDC_BT_ABS, OnBtAbs)
  ON_BN_CLICKED(IDC_BT_TAN, OnBtTan)
  ON_BN_CLICKED(IDC_BT_COS, OnBtCos)
  ON_BN_CLICKED(IDC_BT_LN, OnBtLn)
  ON_BN_CLICKED(IDC_BT_LOG, OnBtLog)
  ON_BN_CLICKED(IDC_BT_EXP, OnBtExp)
  ON_BN_CLICKED(IDC_BT_SQRT, OnBtSqrt)
  ON_LBN_DBLCLK(IDC_LB_OPERAND, OnDblClkOperand)
  ON_BN_CLICKED(IDC_BT_ASIN, OnBtArcSin)
  ON_BN_CLICKED(IDC_BT_ACOS, OnBtArcCos)
  ON_BN_CLICKED(IDC_BT_ATAN, OnBtArcTan)
  ON_BN_CLICKED(IDC_BT_NOT, OnBtNot)
  ON_BN_CLICKED(IDC_BT_SMALLER, OnBtSmaller)
  ON_BN_CLICKED(IDC_BT_GREATER, OnBtGreater)
  ON_BN_CLICKED(IDC_BT_IF, OnBtIf)
  ON_BN_CLICKED(IDC_BT_EQUAL, OnBtEqual) */
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRpnDlgBase message handlers

BOOL CRpnDlgBase::OnInitDialog() {
  CDialog::OnInitDialog();

  // Set the icon for this dialog.  The framework does this automatically
  //  when the application's main window is not a dialog
  SetIcon(m_hIcon, TRUE);  // Set big icon
  SetIcon(m_hIcon, FALSE); // Set small icon

  m_lcOperand.InsertColumn(0, "", LVCFMT_LEFT, LVSCW_AUTOSIZE);

  OnInitComponentBox();
  UpdateSwapButton();

  return TRUE;
}

void CRpnDlgBase::OnInitComponentBox() {
  m_lcOperand.RegisterDropTarget();

  // Fill listbox with item from stack
  CRpnStack::TValueProxyVec vcProxy = m_pStack->ValueProxies();
  for (size_t i = 0; i < vcProxy.size(); i++)
    if (vcProxy[i]->showInDialog()) // wjrx mantis 2521
    {

      new CValueProxyListObject(*this, *vcProxy[i]);
    }

  m_lcOperand.Sort(0);
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CRpnDlgBase::OnPaint() {
  if (IsIconic()) {
    CPaintDC dc(this); // device context for painting

    SendMessage(WM_ICONERASEBKGND, (WPARAM)dc.GetSafeHdc(), 0);

    // Center icon in client rectangle
    int cxIcon = GetSystemMetrics(SM_CXICON);
    int cyIcon = GetSystemMetrics(SM_CYICON);
    CRect rect;
    GetClientRect(&rect);
    int x = (rect.Width() - cxIcon + 1) / 2;
    int y = (rect.Height() - cyIcon + 1) / 2;

    // Draw the icon
    dc.DrawIcon(x, y, m_hIcon);
  } else {
    CDialog::OnPaint();
  }
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CRpnDlgBase::OnQueryDragIcon() { return (HCURSOR)m_hIcon; }

CRpnStack &CRpnDlgBase::RpnStack() { return *m_pStack; }

const CRpnStack &CRpnDlgBase::RpnStack() const { return *m_pStack; }

void CRpnDlgBase::OnBtZero() {
  RpnStack().Constant().Zero();
  UpdateData(FALSE);
}

void CRpnDlgBase::OnBtOne() {
  RpnStack().Constant().One();
  UpdateData(FALSE);
}

void CRpnDlgBase::OnBtTwo() {
  RpnStack().Constant().Two();
  UpdateData(FALSE);
}

void CRpnDlgBase::OnBtThree() {
  RpnStack().Constant().Three();
  UpdateData(FALSE);
}

void CRpnDlgBase::OnBtFour() {
  RpnStack().Constant().Four();
  UpdateData(FALSE);
}

void CRpnDlgBase::OnBtFive() {
  RpnStack().Constant().Five();
  UpdateData(FALSE);
}

void CRpnDlgBase::OnBtSix() {
  RpnStack().Constant().Six();
  UpdateData(FALSE);
}

void CRpnDlgBase::OnBtSeven() {
  RpnStack().Constant().Seven();
  UpdateData(FALSE);
}

void CRpnDlgBase::OnBtEight() {
  RpnStack().Constant().Eight();
  UpdateData(FALSE);
}

void CRpnDlgBase::OnBtNine() {
  RpnStack().Constant().Nine();
  UpdateData(FALSE);
}

void CRpnDlgBase::OnBtPoint() {
  RpnStack().Constant().Point();
  UpdateData(FALSE);
}

void CRpnDlgBase::OnBtSubOrSign() {
  if (RpnStack().IsConstant()) {
    RpnStack().Constant().Sign();
    UpdateData(FALSE);
  } else if (RpnStack().StackSize() > 1) {
    new CRpnBinaryOperation(RpnStack(), CRpnBinaryOperation::OP_SUB);
    UpdateData(FALSE);
  }
}

void CRpnDlgBase::OnBtSign() {
  if (RpnStack().IsConstant()) {
    RpnStack().Constant().Sign();
    UpdateData(FALSE);
  }
}

void CRpnDlgBase::OnBtScn() {
  if (RpnStack().IsConstant()) {
    RpnStack().Constant().Scn();
    UpdateData(FALSE);
  }
}

void CRpnDlgBase::OnBtEnter() { RpnStack().Enter(); }

void CRpnDlgBase::OnBtAdd() {
  if (RpnStack().StackSize() > 1) {
    new CRpnBinaryOperation(RpnStack(), CRpnBinaryOperation::OP_ADD);
    UpdateData(FALSE);
  }
}

void CRpnDlgBase::OnBtSub() {
  if (RpnStack().StackSize() > 1) {
    new CRpnBinaryOperation(RpnStack(), CRpnBinaryOperation::OP_SUB);
    UpdateData(FALSE);
  }
}

void CRpnDlgBase::OnBtPow() {
  if (RpnStack().StackSize() > 1) {
    new CRpnBinaryOperation(RpnStack(), CRpnBinaryOperation::OP_POW);
    UpdateData(FALSE);
  }
}

void CRpnDlgBase::OnBtMult() {
  if (RpnStack().StackSize() > 1) {
    new CRpnBinaryOperation(RpnStack(), CRpnBinaryOperation::OP_MUL);
    UpdateData(FALSE);
  }
}

void CRpnDlgBase::OnBtDiv() {
  if (RpnStack().StackSize() > 1) {
    new CRpnBinaryOperation(RpnStack(), CRpnBinaryOperation::OP_DIV);
    UpdateData(FALSE);
  }
}

void CRpnDlgBase::OnBtClear() {
  RpnStack().Clear(RpnStack());
  UpdateData(FALSE);
}

void CRpnDlgBase::OnBtClearAll() {
  while (RpnStack().StackSize() != 0) {
    RpnStack().Clear(RpnStack());
  };
  UpdateData(FALSE);
}

void CRpnDlgBase::OnBtSin() {
  if (RpnStack().StackSize() > 0) {
    new CRpnUnaryOperation(RpnStack(), CRpnUnaryOperation::OP_SIN);
    UpdateData(FALSE);
  }
}

void CRpnDlgBase::OnBtAbs() {
  if (RpnStack().StackSize() > 0) {
    new CRpnUnaryOperation(RpnStack(), CRpnUnaryOperation::OP_ABS);
    UpdateData(FALSE);
  }
}

void CRpnDlgBase::OnBtTan() {
  if (RpnStack().StackSize() > 0) {
    new CRpnUnaryOperation(RpnStack(), CRpnUnaryOperation::OP_TAN);
    UpdateData(FALSE);
  }
}

void CRpnDlgBase::OnBtCos() {
  if (RpnStack().StackSize() > 0) {
    new CRpnUnaryOperation(RpnStack(), CRpnUnaryOperation::OP_COS);
    UpdateData(FALSE);
  }
}

void CRpnDlgBase::OnBtLn() {
  if (RpnStack().StackSize() > 0) {
    new CRpnUnaryOperation(RpnStack(), CRpnUnaryOperation::OP_LN);
    UpdateData(FALSE);
  }
}

void CRpnDlgBase::OnBtLog() {
  if (RpnStack().StackSize() > 0) {
    new CRpnUnaryOperation(RpnStack(), CRpnUnaryOperation::OP_LOG);
    UpdateData(FALSE);
  }
}

void CRpnDlgBase::OnBtExp() {
  if (RpnStack().StackSize() > 0) {
    new CRpnUnaryOperation(RpnStack(), CRpnUnaryOperation::OP_EXP);
    UpdateData(FALSE);
  }
}

void CRpnDlgBase::OnBtSqrt() {
  if (RpnStack().StackSize() > 0) {
    new CRpnUnaryOperation(RpnStack(), CRpnUnaryOperation::OP_SQRT);
    UpdateData(FALSE);
  }
}

void CRpnDlgBase::OnBtNotEqual() {
  if (RpnStack().StackSize() > 1) {
    new CRpnBinaryOperation(RpnStack(), CRpnBinaryOperation::OP_NOT_EQUAL);
    UpdateData(FALSE);
  }
}

void CRpnDlgBase::OnBtSmallerEqual() {
  if (RpnStack().StackSize() > 1) {
    new CRpnBinaryOperation(RpnStack(), CRpnBinaryOperation::OP_LESS_EQUAL);
    UpdateData(FALSE);
  }
}

void CRpnDlgBase::OnBtGreaterEqual() {
  if (RpnStack().StackSize() > 1) {
    new CRpnBinaryOperation(RpnStack(), CRpnBinaryOperation::OP_GREATER_EQUAL);
    UpdateData(FALSE);
  }
}

void CRpnDlgBase::OnBtSinh() {
  if (RpnStack().StackSize() > 0) {
    new CRpnUnaryOperation(RpnStack(), CRpnUnaryOperation::OP_SINH);
    UpdateData(FALSE);
  }
}

void CRpnDlgBase::OnBtCosh() {
  if (RpnStack().StackSize() > 0) {
    new CRpnUnaryOperation(RpnStack(), CRpnUnaryOperation::OP_COSH);
    UpdateData(FALSE);
  }
}

void CRpnDlgBase::OnBtTanh() {
  if (RpnStack().StackSize() > 0) {
    new CRpnUnaryOperation(RpnStack(), CRpnUnaryOperation::OP_TANH);
    UpdateData(FALSE);
  }
}

void CRpnDlgBase::OnBtRound() {
  if (RpnStack().StackSize() > 0) {
    new CRpnUnaryOperation(RpnStack(), CRpnUnaryOperation::OP_ROUND);
    UpdateData(FALSE);
  }
}

void CRpnDlgBase::OnBtSignFunction() {
  if (RpnStack().StackSize() > 0) {
    new CRpnUnaryOperation(RpnStack(), CRpnUnaryOperation::OP_SIGN);
    UpdateData(FALSE);
  }
}

void CRpnDlgBase::OnBtMin() {
  if (RpnStack().StackSize() > 1) {
    new CRpnBinaryOperation(RpnStack(), CRpnBinaryOperation::OP_MIN);
    UpdateData(FALSE);
  }
}

void CRpnDlgBase::OnBtMax() {
  if (RpnStack().StackSize() > 1) {
    new CRpnBinaryOperation(RpnStack(), CRpnBinaryOperation::OP_MAX);
    UpdateData(FALSE);
  }
}

void CRpnDlgBase::OnBtPI() {
  new CRpnConstant(RpnStack(), CRpnConstant::CON_PI);
  UpdateData(FALSE);
}

void CRpnDlgBase::OnConvertUnit(CButton &button) {
  assert(!m_vcUnitConverters.empty());

  button.SetCheck(1);

  CMenu menu;
  menu.CreatePopupMenu();

  for (size_t i = 0; i < m_vcUnitConverters.size(); ++i)
    menu.AppendMenu(MF_STRING, WM_USER + COMMAND_OFFSET + m_vcUnitConverters[i].first,
                    m_vcUnitConverters[i].second.toStdString().c_str());

  CRect rect;
  button.GetWindowRect(&rect);
  menu.TrackPopupMenu(TPM_LEFTALIGN, rect.BottomRight().x, rect.TopLeft().y, this);

  button.SetCheck(0);
}

BOOL CRpnDlgBase::OnCommand(WPARAM wParam, LPARAM lParam) {
  unsigned int nID = wParam;
  unsigned int nConverterID = nID - WM_USER - COMMAND_OFFSET;

  if (nConverterID > 0 && nConverterID <= m_vcUnitConverters.size()) {
    InsertUnitConverter(CRpnUnitConverter::TConversion(nConverterID));
    return TRUE;
  }

  return CDialog::OnCommand(wParam, lParam);
}

void CRpnDlgBase::InsertUnitConverter(CRpnUnitConverter::TConversion conversion) {
  if (RpnStack().StackSize() > 0) {
    new CRpnUnitConverter(RpnStack(), conversion);
    UpdateData(FALSE);
  }
}

void CRpnDlgBase::OnOK() {
  switch (RpnStack().StackSize()) {
  case 0:
    _m()->msg("Please, insert a formula");
    return;
    break;
  case 1:
    break;
  default:
    _m()->msg("Formula stack must contain one term");
    return;
  }

  CDialog::OnOK();
}

void CRpnDlgBase::OnBtArcSin() {
  if (RpnStack().StackSize() > 0) {
    new CRpnUnaryOperation(RpnStack(), CRpnUnaryOperation::OP_ASIN);
    UpdateData(FALSE);
  }
}

void CRpnDlgBase::OnBtArcCos() {
  if (RpnStack().StackSize() > 0) {
    new CRpnUnaryOperation(RpnStack(), CRpnUnaryOperation::OP_ACOS);
    UpdateData(FALSE);
  }
}

void CRpnDlgBase::OnBtArcTan() {
  if (RpnStack().StackSize() > 0) {
    new CRpnUnaryOperation(RpnStack(), CRpnUnaryOperation::OP_ATAN);
    UpdateData(FALSE);
  }
}

void CRpnDlgBase::OnBtNot() {
  if (RpnStack().StackSize() > 0) {
    new CRpnUnaryOperation(RpnStack(), CRpnUnaryOperation::OP_NOT);
    UpdateData(FALSE);
  }
}

void CRpnDlgBase::OnBtSmaller() {
  if (RpnStack().StackSize() > 1) {
    new CRpnBinaryOperation(RpnStack(), CRpnBinaryOperation::OP_LESS);
    UpdateData(FALSE);
  }
}

void CRpnDlgBase::OnBtGreater() {
  if (RpnStack().StackSize() > 1) {
    new CRpnBinaryOperation(RpnStack(), CRpnBinaryOperation::OP_GREATER);
    UpdateData(FALSE);
  }
}

void CRpnDlgBase::OnBtOr() {
  if (RpnStack().StackSize() > 1) {
    new CRpnBinaryOperation(RpnStack(), CRpnBinaryOperation::OP_OR);
    UpdateData(FALSE);
  }
}

void CRpnDlgBase::OnBtAnd() {
  if (RpnStack().StackSize() > 1) {
    new CRpnBinaryOperation(RpnStack(), CRpnBinaryOperation::OP_AND);
    UpdateData(FALSE);
  }
}

void CRpnDlgBase::OnBtIf() {
  if (RpnStack().StackSize() > 2) {
    new CRpnTernaryOperation(RpnStack(), CRpnTernaryOperation::OP_IF);
    UpdateData(FALSE);
  }
}

void CRpnDlgBase::OnBtEqual() {
  if (RpnStack().StackSize() > 1) {
    new CRpnBinaryOperation(RpnStack(), CRpnBinaryOperation::OP_EQUAL);
    UpdateData(FALSE);
  }
}

BOOL CRpnDlgBase::ValidRpnStack() const { return m_pStack->exists(); }

void CRpnDlgBase::UpdateStackDisplay() {
  m_lcStack.DeleteAllItems();
  int maxwidth = 0;
  for (int i = 0; i < RpnStack().StackSize(); i++) {
    CString sRow;
    sRow.Format("%d: %s", i + 1, RpnStack().StackAt(RpnStack().StackSize() - i - 1).Formula().toStdString().c_str());
    int width = m_lcStack.GetStringWidth(sRow);
    if (width > maxwidth)
      maxwidth = width;
    m_lcStack.InsertItem(i, sRow);
  }
  m_lcStack.DeleteColumn(0);
  m_lcStack.InsertColumn(0, "", LVCFMT_LEFT, maxwidth + 10);
}

void CRpnDlgBase::OnBtSwap() {
  assert(CanSwap());
  std::vector<int> vcSelection;
  for (int i = 0; i < m_lcStack.GetItemCount(); i++) {
    if (m_lcStack.GetItemState(i, LVIS_SELECTED) == LVIS_SELECTED)
      vcSelection.push_back(i);
  }
  assert(vcSelection.size() == 2);
  m_pStack->Swap(m_pStack->StackSize() - 1 - vcSelection[0], m_pStack->StackSize() - 1 - vcSelection[1]);
  UpdateStackDisplay();
  UpdateSwapButton();
}

bool CRpnDlgBase::CanSwap() const { return m_lcStack.GetSelectedCount() == 2; }

void CRpnDlgBase::OnKeyDown(unsigned int nChar, unsigned int nRepCnt, unsigned int nFlags) {
  switch (nChar) {
  case '0':
    OnBtZero();
    break;
  case '1':
    OnBtOne();
    break;
  case '2':
    OnBtTwo();
    break;
  case '3':
    OnBtThree();
    break;
  case '4':
    OnBtFour();
    break;
  case '5':
    OnBtFive();
    break;
  case '6':
    OnBtSix();
    break;
  case '7':
    OnBtSeven();
    break;
  case '8':
    OnBtEight();
    break;
  case '9':
    OnBtNine();
    break;
  case '+':
    OnBtAdd();
    break;
  case '-':
    OnBtSub();
    break;
  case '/':
    OnBtDiv();
    break;
  case '*':
    OnBtMult();
    break;
  case '\n':
    OnBtEnter();
    break;
  case '<':
    OnBtSmaller();
    break;
  case '>':
    OnBtGreater();
    break;
  }
}

} // namespace rpn
