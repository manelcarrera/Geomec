// RPNDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Geomec.h"
#include "RPNDlg.h"
#include "RpnValueSet.h"
#include "ResultSelectDlg.h"
#include "ResultMaterialSelectDlg.h"
#include "ModelBase.h"
#include "DerivedResult.h"
#include "RpnEditFormulaDlg.h"
#include "RPN_INC.h"
#include "RpnMaterialParameterProxy.h"
#include "ValueTypeFactory.h"
#include "MaterialServer.h"
#include "FillMaterialParameterProxies.h"
#include "DerivedResult.h"
#include "TextTagUtils.h"
#include "ResultInfo.h"
#include "GlobalMessage.h"

#ifdef _DEBUG
//#define new DEBUG_NEW
#ifdef _MSC_VER
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif  // _MSC_VER
#endif

namespace {

const CString SINGLE_QUOTE = "\'";

CString stripSingleQuotes(const CString& string)
{
  CString newString = string;

  if (newString.Left(1) == SINGLE_QUOTE) {
    newString.Delete(0);
  }

  if (newString.Right(1) == SINGLE_QUOTE) {
    newString.Delete(strlen((LPCTSTR) newString) - 1);
  }

  return newString;
}

}	// anonymous namespace

/////////////////////////////////////////////////////////////////////////////
// CRpnDlg::CNewItemListObject

CRpnDlg::CNewItemListObject::CNewItemListObject(CRpnDlg& dlg, bool bMaterialItem /*= false*/)
: IListObject(dlg.ListCtrl(), dlg.ListCtrl().GetItemCount(), 0), m_dlg(dlg), m_bMaterialItem(bMaterialItem)
{
  Ctrl().SetItemData(Ctrl().GetItemCount() - 1, (DWORD_PTR) this);
}

QString CRpnDlg::CNewItemListObject::Text() const
{
  if(m_bMaterialItem)
  return "<new material item>";
  
  return "<new result item>";
}

unsigned int CRpnDlg::CNewItemListObject::Icon() const
{
  return IDI_NEW_ITEM;
}

void CRpnDlg::CNewItemListObject::SelectMaterialItem()
{
  // waij TFS 101571 do not allow duplicate material items
  //
  std::set<std::string> stAvailableOperands;
  rpn::CRpnStack::TValueProxyVec valueProxyVector= 
  m_dlg.RpnStack().ValueProxies();

  for (size_t ii= 0; ii< valueProxyVector.size(); ++ii)
  {
  stAvailableOperands.insert( valueProxyVector[ii]->TextTag().toStdString().c_str() );
  }

  CFillMaterialParameterProxies
  fillMaterialParameterProxies(m_dlg.Model().Mesh(), m_dlg.RpnStack());
  std::vector <IRpnMaterialParameterProxy*> matProxies =
  fillMaterialParameterProxies.getMaterialParameterProxies();
  CResultMaterialSelectDlg dlg(m_dlg.Model(), matProxies);
  std::vector <IRpnMaterialParameterProxy*> selectedProxies;
  if(dlg.DoModal() == IDOK) 
  {
  // make the selected proxy visible in the dialog
  selectedProxies = dlg.SelectedProxies();

  for (std::vector <IRpnMaterialParameterProxy*>::const_iterator
      selectedProxy = selectedProxies.begin();
      selectedProxy != selectedProxies.end(); ++selectedProxy)
  {
      bool found=
    stAvailableOperands.find((*selectedProxy)->TextTag().toStdString() )!=
    stAvailableOperands.end();

      for (int i = 0; !found && (i < m_dlg.ListCtrl().GetItemCount()); ++i)
      {
    CNewItemListObject* newItemListObject =
          (CNewItemListObject*) m_dlg.ListCtrl().GetItemData(i);

    found = found || (newItemListObject->Text() ==
          (*selectedProxy)->TextTag());
      }

      if (!found)
      {
    new CValueProxyListObject(m_dlg, *(*selectedProxy), 1);
      }
  }
  }
  // delete the proxies we don't need
  for (size_t i = 0; i < matProxies.size(); ++i)
  {
  // waij TFS 101571 do not allow duplicate material items
  //
  std::set<std::string>::const_iterator nameIt =
      stAvailableOperands.find(matProxies[i]->TextTag().toStdString());

  bool foundAvailable = nameIt != stAvailableOperands.end();
  bool foundCreated= false;

  if (! foundAvailable)
  for (std::vector <IRpnMaterialParameterProxy*>::const_iterator
      selectedProxy = selectedProxies.begin();
      !foundCreated && (selectedProxy != selectedProxies.end());
      ++selectedProxy)
  {
      foundCreated = foundCreated || (matProxies[i] == *selectedProxy);
  }

  if (foundAvailable || !foundCreated )
  {
      delete matProxies[i];
  }
  }
  m_dlg.UpdateStackDisplay();
}

void CRpnDlg::CNewItemListObject::SelectResultItem()
{
  CResultSelectDlg dlg(m_dlg.Model(), m_dlg.StoreGlobal(), m_dlg.Result());
  if(dlg.DoModal() == IDOK)
  {
  // By now the destructor of CResultSelectDlg::CResultGroupObserver has been
  // triggered, destroying the results! Hence the alternative construction.
  // The destructor of CResultSelectDlg::CResultGroupObserver has copied the
  // results to its paretn class CResultSelectDlg.

    // inputs
    if( dlg.m_selected_inputs_v.size() )
    {
    for ( auto &value_component : dlg.m_selected_inputs_v ) 
    {
      std::string name = dlg.Name( value_component ).toStdString();
      if ( CheckUniqueName(name) )
      {
        CDerivedResult::CDerivedResultComponent::CValueComponentProxy* pProxy = 
          new CDerivedResult::CDerivedResultComponent::CValueComponentProxy( 
            name.c_str()
            , const_cast<IValueComponentBase&>( *value_component )
            , m_dlg.RpnStack()
            , dlg.FixedLinStage());

        pProxy->StoreOnFile(!m_dlg.StoreGlobal());
        new CValueProxyListObject(m_dlg, *pProxy, 1);
      }
    }
    }
    else // outputs
    {

    CResultSelectDlg::CResultComponentSetIterator
      resultComponentSetIterator(&dlg,
      CResultSelectDlg::CResultComponentSetIterator::resultsFromDialog);
    const IValueComponentBase* valueComponent = 0;

    while ((valueComponent = resultComponentSetIterator.get()) != 0)
    {
      std::string name = dlg.Name(valueComponent).toStdString();
      if ( CheckUniqueName(name) )
      {
      CDerivedResult::CDerivedResultComponent::CValueComponentProxy*
      pProxy = new CDerivedResult::CDerivedResultComponent::CValueComponentProxy
        ( name.c_str()
        , const_cast<IValueComponentBase&>(*valueComponent)
        , m_dlg.RpnStack()
        , dlg.FixedLinStage()
        );
      pProxy->StoreOnFile(!m_dlg.StoreGlobal());
      new CValueProxyListObject(m_dlg, *pProxy, 1);
      }
    }
    } // end outputs
  }
}

void CRpnDlg::CNewItemListObject::OnDoubleClick()
{
  if(m_bMaterialItem)
  SelectMaterialItem();
  else
  SelectResultItem();
}

bool CRpnDlg::CNewItemListObject::CheckUniqueName(const std::string &name) const
{
  for (size_t ii= 0; ii< m_dlg.RpnStack().ValueProxies().size(); ++ii)
  {
  if (name == m_dlg.RpnStack().ValueProxies()[ii]->TextTag().toStdString())
  {
      return false;
  }
  }
  return true;
}
/////////////////////////////////////////////////////////////////////////////
// CRpnDlg::CUnAssignedListObject

CRpnDlg::CUnAssignedListObject::CUnAssignedListObject(const CString& sDescription, CRpnDlg& dlg)
: IListObject(dlg.ListCtrl(), dlg.ListCtrl().GetItemCount(), 0), m_dlg(dlg), m_sDescription(sDescription)
{
  Ctrl().SetItemData(Ctrl().GetItemCount() - 1, (DWORD_PTR) this);
}

QString CRpnDlg::CUnAssignedListObject::Text() const
{
  return QString("<") + (LPCSTR) m_sDescription + ">";
}

unsigned int CRpnDlg::CUnAssignedListObject::Icon() const
{
  return IDI_UNDEFINED;
}

DROPEFFECT CRpnDlg::CUnAssignedListObject::CanDrop(TCtrlObjectVec &vcDragged, BOOL bMove) const
{
  for(size_t i = 0; i < vcDragged.size(); i++)
  {
    CValueProxyListObject *pProxy = dynamic_cast<CValueProxyListObject*>(vcDragged[i]);
    if(pProxy) return DROPEFFECT_COPY;
  }

  return DROPEFFECT_NONE;
}

void CRpnDlg::CUnAssignedListObject::Drop(TCtrlObjectVec &vcDragged, BOOL bMove)
{
  for(size_t i = 0; i < vcDragged.size(); i++)
  {
    CValueProxyListObject *pProxy = dynamic_cast<CValueProxyListObject*>(vcDragged[i]);
    if(pProxy)
    {
      m_dlg.RpnStack().SetUnassignedOperand((LPCSTR) m_sDescription, pProxy->Proxy());
      m_dlg.UpdateStackDisplay();
      delete this;
      return;
    }
  }
}

/////////////////////////////////////////////////////////////////////////////
// CRpnDlg dialog

CRpnDlg::CRpnDlg(CRpnValueSet& value_set, CWnd* pParent)
: CRpnDlgBase(value_set.RpnStack(), CRpnDlg::IDD, pParent),
  m_sName(value_set.Name().toStdString().c_str()),
  m_pResult(0),
  m_pValueSet(&value_set)
{
  m_sName.TrimLeft(" ");
  if(m_sName.IsEmpty())
    m_bNameEqualToFormula = TRUE;
  else
    m_bNameEqualToFormula = stripSingleQuotes(m_sName).CompareNoCase(RpnStack().Formula().toStdString().c_str()) == 0;

  //{{AFX_DATA_INIT(CRpnDlg)
  //}}AFX_DATA_INIT
  // Note that LoadIcon does not require a subsequent DestroyIcon in Win32
}

CRpnDlg::CRpnDlg(CDerivedResult::CDerivedResultComponent& result, CWnd* pParent)	
: CRpnDlgBase(result.RpnStack(), CRpnDlg::IDD, pParent), 
  m_sName(result.Name().toStdString().c_str()),
  m_pResult(&result),
  m_pValueSet(0)
{
  m_sName.TrimLeft(" ");
  if(m_sName.IsEmpty())
    m_bNameEqualToFormula = TRUE;
  else
    m_bNameEqualToFormula = stripSingleQuotes(m_sName).CompareNoCase(RpnStack().Formula().toStdString().c_str()) == 0;
}

CRpnDlg::~CRpnDlg()
{
}

void CRpnDlg::DoDataExchange(CDataExchange* pDX)
{
  CRpnDlgBase::DoDataExchange(pDX);

  //{{AFX_DATA_MAP(CRpnDlg)
  DDX_Control(pDX, IDC_LC_COMPONENT, m_lcOperand);
  DDX_Control(pDX, IDC_LC_STACK, m_lcStack);
  //}}AFX_DATA_MAP

  if(!pDX->m_bSaveAndValidate)
  {
    UpdateStackDisplay();
    UpdateEditFormulaButton();

    // wjrx mantis 2583 Put ' ' around the rpn formula name.
    if(m_bNameEqualToFormula)
    {
      m_sName= RpnStack().Formula().toStdString().c_str();
      int length= m_sName.GetLength();
      if ( length > 0)
      {
        if ( m_sName[length-1] == ' ' )
          m_sName.SetAt(length-1, '\'');
        else
          m_sName= m_sName + SINGLE_QUOTE;
        m_sName= SINGLE_QUOTE + m_sName;
      }
      else
        m_sName= "\'\'";
    }
    
    DDX_Text(pDX, IDC_EB_NAME, m_sName);
    DDX_Check(pDX, IDC_CK_NAME, m_bNameEqualToFormula);
  }

  if(pDX->m_bSaveAndValidate)
  {
    DDX_Text(pDX, IDC_EB_NAME, m_sName);

    // wjrx mantis 2583
    // Replace all double quotes " with single quotes '
    // Reloading the formula from registry crashes on ".
    //
    int pos, oldpos= 0;
    while ( ( pos= m_sName.Find( "\"", oldpos) )  > -1 )
    {
      m_sName.SetAt(pos, '\'');
      oldpos= pos;
    }

    rpn::CRpnStack::TValueProxyVec vcProxy = RpnStack().ValueProxies();
    bool bInUse = false;
    for(size_t i = 0; i < vcProxy.size(); i++)
    {
      if(vcProxy[i]->TextTag() == m_sName)
        bInUse = true;
    }
    
    if(bInUse)
    {
      // We fail
      _m()->msg("Name already in use");
      pDX->Fail();
    }

    rpn::CRpnObject::TParentSet stParent;
    if(m_pValueSet)
      stParent.insert(&m_pValueSet->RpnStack());

    if(m_pResult)
      stParent.insert(&m_pResult->RpnStack());

    if(RpnStack().Recursive(stParent))
    {
      _m()->msg("Formula is recursive");
      pDX->Fail();
    }

    if(m_pResult)
    {
      m_pResult->RpnStack(RpnStack());
      std::list<std::string> lstMessages;
      m_pResult->RecursiveExpand(lstMessages);
      m_pResult->Name((LPCSTR) m_sName);

      if(!lstMessages.empty())
      {
    CString msg;
    for(std::list<std::string>::const_iterator it = lstMessages.begin(); it != lstMessages.end(); ++it)
          msg += CString((*it).c_str()) + "\n";

    AfxMessageBox(msg);
      }

      m_pResult->Modified();
    }

    if(m_pValueSet)
    {
      m_pValueSet->Name((LPCSTR) m_sName);
      m_pValueSet->RpnStack(RpnStack());
    }
  }
}

BEGIN_MESSAGE_MAP(CRpnDlg, CDialog)
  //{{AFX_MSG_MAP(CRpnDlg)
  ON_EN_CHANGE(IDC_EB_NAME, OnNameChange)
  ON_BN_CLICKED(IDC_CK_NAME, OnCkName)
  ON_WM_PAINT()
  ON_WM_QUERYDRAGICON()
  ON_BN_CLICKED(IDC_BT_0, OnBtZero)
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
  ON_BN_CLICKED(IDC_BT_SIGN, OnBtSign)
  ON_BN_CLICKED(IDC_BT_ENTER, OnBtEnter)
  ON_BN_CLICKED(IDC_BT_ADD, OnBtAdd)
  ON_BN_CLICKED(IDC_BT_CLEAR, OnBtClear)
  ON_BN_CLICKED(IDC_BT_CLEAR_ALL, OnBtClearAll)
  ON_BN_CLICKED(IDC_BT_PI, OnBtPI)
  ON_BN_CLICKED(IDC_BT_POW, OnBtPow)
  ON_BN_CLICKED(IDC_BT_SCN, OnBtScn)
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
  ON_BN_CLICKED(IDC_BT_ASIN, OnBtArcSin)
  ON_BN_CLICKED(IDC_BT_ACOS, OnBtArcCos)
  ON_BN_CLICKED(IDC_BT_ATAN, OnBtArcTan)
  ON_BN_CLICKED(IDC_BT_NOT, OnBtNot)
  ON_BN_CLICKED(IDC_BT_SMALLER, OnBtSmaller)
  ON_BN_CLICKED(IDC_BT_GREATER, OnBtGreater)
  ON_BN_CLICKED(IDC_BT_IF, OnBtIf)
  ON_BN_CLICKED(IDC_BT_EQUAL, OnBtEqual) 
  ON_BN_CLICKED(IDC_BT_OR, OnBtOr) 
  ON_BN_CLICKED(IDC_BT_AND, OnBtAnd)
  ON_WM_KEYDOWN()
  ON_WM_RBUTTONDOWN()
  ON_BN_CLICKED(IDC_BT_NOTEQUAL, OnBtNotEqual)
  ON_BN_CLICKED(IDC_BT_SMALLEREQUAL, OnBtSmallerEqual)
  ON_BN_CLICKED(IDC_BT_GREATEREQUAL, OnBtGreaterEqual)
  ON_BN_CLICKED(IDC_BT_TANH, OnBtTanh)
  ON_BN_CLICKED(IDC_BT_COSH, OnBtCosh)
  ON_BN_CLICKED(IDC_BT_SINH, OnBtSinh)
  ON_BN_CLICKED(IDC_BT_MIN, OnBtMin)
  ON_BN_CLICKED(IDC_BT_MAX, OnBtMax)
  ON_BN_CLICKED(IDC_BT_ROUND, OnBtRound)
  ON_BN_CLICKED(IDC_BT_SWAP, OnBtSwap)
  ON_BN_CLICKED(IDC_BT_EDIT_FORMULA, OnBtEditFormula) //mantis 2433 wjrx
  ON_BN_CLICKED(IDC_BT_SIGN_FUNC, OnBtSignFunction)
  ON_BN_CLICKED(IDC_CONVERT_UNIT, OnBtConvertUnit)
  ON_NOTIFY(LVN_ITEMCHANGED, IDC_LC_STACK, OnStackItemChanged)
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRpnDlg message handlers

// mantis 2433 wjrx
void CRpnDlg::OnBtEditFormula() 
{
  std::queue<Shunt::CShunt::Op_t> output;
  std::set<std::string> operandNames;
  rpn::CRpnStack::TValueProxyVec valueProxyVector= RpnStack().ValueProxies();

  const std::set<std::string> &matProxyNames=
  CFillMaterialParameterProxies::GetNames( Model());

  // All valid materialparameter names, these can be typed directly into the
  // Edit Formula dialog.
  //
  operandNames= matProxyNames;


  // the items in IDC_LC_COMPONENT, the materials/results listbox
  //
  for (size_t ii= 0; ii< valueProxyVector.size(); ++ii)
  {
  operandNames.insert(valueProxyVector[ii]->TextTag().toStdString());
  }

  // names of the mesh results
  operandNames.insert
  ( CMeshResultTree::getNames().begin()
  , CMeshResultTree::getNames().end()
  );

  CRpnEditFormulaDlg
  dlg( Model()
       , RpnStack().Formula().toStdString()
       , operandNames
       , output
       );

  if ( dlg.DoModal() != IDOK ) return;
  OnBtClearAll();

  while ( output.size() > 0 ) 
  {
  Shunt::CShunt::Op_t word= output.front();
  assert( word.data.size() > 0 );

  output.pop();

  if ( word.type == Shunt::CShunt::E_OPAND ) // operand
  {
      OnBtEnter();

      if ( Shunt::CShunt::validFloatingPoint( word.data ) )
      {
    //number
    for (size_t ii= 0; ii< word.data.size(); ++ii)
    {
          switch(word.data.substr(ii,1)[0])
          {
          case '0': OnBtZero(); break;
          case '1': OnBtOne(); break;
          case '2': OnBtTwo(); break;
          case '3': OnBtThree(); break;
          case '4': OnBtFour(); break;
          case '5': OnBtFive(); break;
          case '6': OnBtSix(); break;
          case '7': OnBtSeven(); break;
          case '8': OnBtEight(); break;
          case '9': OnBtNine(); break;
          case '.': OnBtPoint(); break;
          case 'e': OnBtScn(); break;  // 2e3
          case '-': OnBtSign(); break; // 2e-3
          default: assert(0); //invalid 
          }
    }
      }
      else // named operand
      {
    bool foundIt= false;

    if ( word.data == "pi" ) //wjrx mantis 2558
    {
          OnBtPI(); // the only named operand handle with its own button
          foundIt= true;
    }
    else
    {
          for (size_t ii= 0; ii< valueProxyVector.size(); ++ii)
          {
      if ( word.data ==
                 valueProxyVector[ii]->TextTag().toStdString())
      {
              // Identical to 
              // void CRpnDlgBase::CValueProxyListObject::OnDoubleClick()
              new rpn::CRpnOperand(RpnStack(),*(RpnStack().ValueProxies()[ii]));
              UpdateData(FALSE);
              foundIt= true;
              break;
      }
          }//for

          // waij TFS 92432
          // This operand does not come from clicking an item in
          // IDC_LC_COMPONENT but from editing the formula. 
          //
          if ( foundIt==false) // attempt to create material/result item
          {
      IRpnMaterialParameterProxy *newproxy1= 
              CFillMaterialParameterProxies::CreateMaterialParameterProxy
        (Model(),word.data,RpnStack());

      if ( foundIt= newproxy1!=0 )
      {
              new rpn::CRpnOperand(RpnStack(), *newproxy1);	
              new CValueProxyListObject(*this, *newproxy1, 1);
              UpdateData(FALSE);
      }
      else // attempt to create result item
      {
              CResultInfo resultInfo;
              if (! Model().ResultTree().ValidName(word.data,resultInfo) )
        Model().MeshResultTree().ValidName(word.data,resultInfo);
              if ( resultInfo.Result() )
              {
        const IValueComponentBase &component= 
                  resultInfo.Result()->Component(resultInfo.Component(),0);
        CDerivedResult::CDerivedResultComponent::CValueComponentProxy*
                  newproxy2 = new CDerivedResult::CDerivedResultComponent::
                  CValueComponentProxy
          ( word.data.c_str()
          , const_cast<IValueComponentBase&>(component)
          , RpnStack()
          , resultInfo.TimeStep()!=-1
          );
        new rpn::CRpnOperand(RpnStack(), *newproxy2);	
        newproxy2->StoreOnFile(!StoreGlobal());
        new CValueProxyListObject(*this, *newproxy2, 1);
        UpdateData(FALSE);
        foundIt= true;
              }
              else
        assert(0);
      }
          }
    } // named operand but not pi

    assert(foundIt); // we should not get an unknown named operand
      } //else named operand
  } //if operand
  else if ( word.type == Shunt::CShunt::E_UNOPER ) // unary operator
  {
      if ( word.data == "-" ) OnBtSign();
  }
  else // binary operator or function
  {
           if ( word.data == "arcsin" ) OnBtArcSin();
      else if ( word.data == "arccos" ) OnBtArcCos();
      else if ( word.data == "arctan" ) OnBtArcTan();
      else if ( word.data == "round" ) OnBtRound();
      else if ( word.data == "if" ) {;} //load stack and wait for 'else'
      else if ( word.data == "then" ) {;} //load stack and wait for 'else'
      else if ( word.data == "else" ) OnBtIf();
      else if ( word.data == "sign" ) OnBtSignFunction();
      else if ( word.data == "sqrt" ) OnBtSqrt();
      else if ( word.data == "sinh" ) OnBtSinh();
      else if ( word.data == "cosh" ) OnBtCosh();
      else if ( word.data == "tanh" ) OnBtTanh();
      else if ( word.data == "abs" ) OnBtAbs();
      else if ( word.data == "exp" ) OnBtExp();
      else if ( word.data == "sin" ) OnBtSin();
      else if ( word.data == "cos" ) OnBtCos();
      else if ( word.data == "tan" ) OnBtTan();
      else if ( word.data == "log" ) OnBtLog();
      else if ( word.data == "ln" ) OnBtLn();

      else if ( word.data == "ft_m" ) InsertUnitConverter(rpn::CRpnUnitConverter::FT_M);
      else if ( word.data == "m_ft" ) InsertUnitConverter(rpn::CRpnUnitConverter::M_FT);
      else if ( word.data == "ppg_kgm3" ) InsertUnitConverter(rpn::CRpnUnitConverter::PPG_KGPM3);
      else if ( word.data == "kgm3_ppg" ) InsertUnitConverter(rpn::CRpnUnitConverter::KGPM3_PPG);
      else if ( word.data == "C_K" ) InsertUnitConverter(rpn::CRpnUnitConverter::C_K);
      else if ( word.data == "K_C" ) InsertUnitConverter(rpn::CRpnUnitConverter::K_C);
      else if ( word.data == "F_C" ) InsertUnitConverter(rpn::CRpnUnitConverter::F_C);
      else if ( word.data == "C_F" ) InsertUnitConverter(rpn::CRpnUnitConverter::C_F);
      else if ( word.data == "psi_MPa" ) InsertUnitConverter(rpn::CRpnUnitConverter::PSI_MPA);
      else if ( word.data == "MPa_psi" ) InsertUnitConverter(rpn::CRpnUnitConverter::MPA_PSI);

      else if ( word.data == "not" ) OnBtNot();
      else if ( word.data == "and" ) OnBtAnd();
      else if ( word.data == "or" ) OnBtOr(); 
      else if ( word.data == "min" ) OnBtMin();
      else if ( word.data == "max" ) OnBtMax();
      else if ( word.data == "pow" ) OnBtPow();
      else if ( word.data == "!=" ) OnBtNotEqual();
      else if ( word.data == "<=" ) OnBtSmallerEqual();
      else if ( word.data == "<" ) OnBtSmaller();
      else if ( word.data == ">=" ) OnBtGreaterEqual();
      else if ( word.data == ">" ) OnBtGreater();
      else if ( word.data == "=" ) OnBtEqual(); 
      else if ( word.data == "+" ) OnBtAdd();
      else if ( word.data == "/" ) OnBtDiv();
      else if ( word.data == "*" ) OnBtMult();
      else if ( word.data == "-" ) OnBtSub();
  }
  }

  UpdateStackDisplay();

  //Finish with enter, in case the last entry was an operand.
  //
  OnBtEnter();
}

void CRpnDlg::OnNameChange() 
{
  if(m_bNameEqualToFormula)
  {
    m_bNameEqualToFormula = FALSE;
    CButton* pButton = (CButton*)GetDlgItem(IDC_CK_NAME);
    pButton->SetCheck(0);
  }
  GetDlgItem(IDC_EB_NAME)->GetWindowText(m_sName);
}

void CRpnDlg::OnCkName()
{
  if(!m_bNameEqualToFormula)
  {
    m_sName = RpnStack().Formula().toStdString().c_str();
  }

  m_bNameEqualToFormula = !m_bNameEqualToFormula;

  UpdateData(FALSE);
}

void CRpnDlg::OnInitComponentBox()
{
  if(m_pValueSet == 0)
  {
    new CNewItemListObject(*this, false);
  new CNewItemListObject(*this, true);
  }
  
  std::vector<QString> vcUnassigned = RpnStack().UnAssignedOperands();
  for(size_t i = 0; i < vcUnassigned.size(); i++)
    new CUnAssignedListObject(vcUnassigned[i].toStdString().c_str(), *this);

  CRpnDlgBase::OnInitComponentBox();
}

void CRpnDlg::OnBtConvertUnit()
{
  CButton* pButton = (CButton*)GetDlgItem(IDC_CONVERT_UNIT);
  assert(pButton);
  if(pButton)
  OnConvertUnit(*pButton);
}

BOOL CRpnDlg::StoreGlobal() const
{
  if(m_pValueSet)
    return FALSE;

  return !((CDerivedResult&)m_pResult->Parent()).StoreOnFile();
}

const CModelBase& CRpnDlg::Model() const
{
  if(m_pValueSet)
    return dynamic_cast<const CModelBase&>(m_pValueSet->Model());

  return dynamic_cast<const CModelBase&>(m_pResult->Model());
}

CModelBase& CRpnDlg::Model()
{
  if(m_pValueSet)
    return dynamic_cast<CModelBase&>(m_pValueSet->Model());

  return dynamic_cast<CModelBase&>(m_pResult->Model());
}

const CDerivedResult::CDerivedResultComponent* CRpnDlg::Result() const
{
  return m_pResult;
}

void CRpnDlg::UpdateSwapButton()
{
  GetDlgItem(IDC_BT_SWAP)->EnableWindow(m_lcStack.GetSelectedCount() == 2);
}

void CRpnDlg::UpdateEditFormulaButton()
{
  GetDlgItem(IDC_BT_EDIT_FORMULA)->EnableWindow(m_lcStack.GetItemCount() < 2);
}

void CRpnDlg::OnStackItemChanged(NMHDR* pNMHDR, LRESULT* pResult) 
{
  NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

  UpdateSwapButton();
  
  *pResult = 0;
}

BOOL CRpnDlg::PreTranslateMessage(MSG* pMsg) //wjrx mantis 686
{
    // <return> only passes as a WM_KEYDOWN msg. Some keys pass as both
    // WM_CHAR and WM_KEYDOWN, potentially with different wParam values.
    //
  if (   pMsg->message == WM_CHAR )
  {
    CWnd* pWnd = GetFocus();
    if ( GetDlgItem(IDC_EB_NAME) != GetFocus() ) //not line edit
    {
      switch( pMsg->wParam)
            {
            case '0':         OnBtZero();  return 1;
            case '1':         OnBtOne();   return 1;
            case '2':         OnBtTwo();   return 1;
            case '3':         OnBtThree(); return 1;
            case '4':         OnBtFour();  return 1;
            case '5':         OnBtFive();  return 1;
            case '6':         OnBtSix();   return 1;
            case '7':         OnBtSeven(); return 1;
            case '8':         OnBtEight(); return 1;
            case '9':         OnBtNine();  return 1;
            case '.':         OnBtPoint(); return 1;
            case '+':         OnBtAdd();   return 1;
            case '-':         OnBtSubOrSign(); return 1;
            case '/':         OnBtDiv();   return 1;
            case '*':         OnBtMult();  return 1;
            case '\b':        OnBtClear(); return 1;
            }
    }
  }
    else if ( pMsg->message == WM_KEYDOWN )
  {
    CWnd* pWnd = GetFocus();
    if ( GetDlgItem(IDC_EB_NAME) != GetFocus() ) //not line edit
    {
      switch( pMsg->wParam)
            {
            case '\r':        OnBtEnter(); return 1;
            }
    }
  }

  return CDialog::PreTranslateMessage(pMsg);
}
