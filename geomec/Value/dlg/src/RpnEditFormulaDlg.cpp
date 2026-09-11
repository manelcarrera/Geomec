// RpnEditFormulaDlg.cpp : implementation file
//

#include "stdafx.h"
#include "geomec.h"
#include "RpnEditFormulaDlg.h"
#include "RPN_INC.h"
#include <string>
#include <iostream>
#include <queue>
#include <set>
#include "ModelBase.h"
#include "TextTagUtils.h"
#include "ResultInfo.h"
#include "ResultTree.h"
#include "MeshResultTree.h"
#include "FillMaterialParameterProxies.h"

#ifdef _DEBUG
//#define new DEBUG_NEW
#ifdef _MSC_VER
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif  // _MSC_VER
#endif

/////////////////////////////////////////////////////////////////////////////
// CRpnEditFormulaDlg dialog


CRpnEditFormulaDlg::CRpnEditFormulaDlg
  ( const CModelBase &model
  , const std::string & input
  , const std::set<std::string> & operandNames
  , std::queue<Shunt::CShunt::Op_t> &output
  , CWnd* pParent /*=NULL*/
  )
  : CDialog(CRpnEditFormulaDlg::IDD, pParent)
  , m_model(model)
  , m_sInfixFormula( input)
  , m_qsRPNFormula( output)
  , m_bValidatedOk(false)
  , m_bCaughtException(false)
{

  if ( m_bCaughtException==true ) return;

  // this is a named operand, but defined in the calculator
  // and handled by a button.
  addOperandName( "pi" ); //wjrx mantis 2558

  try
  {
  for ( std::set<std::string>::const_iterator it= operandNames.begin()
    ; it != operandNames.end()
    ; ++it
    )
  {
      addOperandName(*it);
  }

  }
  catch(...)
  {
  m_bCaughtException= true;
  }
}

void CRpnEditFormulaDlg::addOperandName( const std::string & operandName)
{
  m_namedOperands.insert( operandName );
}

bool CRpnEditFormulaDlg::checkOperandNames
( const std::list<Shunt::CRpnToken *> & tokens)
{
  std::list<Shunt::CRpnToken *>::const_iterator it;

  CResultInfo resultInfo;

  int pos= 0;
  for ( it= tokens.begin(); it != tokens.end(); ++it )
  {
  ++pos;
  Shunt::CRpnOperand *opand= dynamic_cast<Shunt::CRpnOperand *>( *it);
  if ( ! opand ) continue;
  if ( Shunt::CShunt::validFloatingPoint( opand->getValue() ) ) continue;
  if ( m_namedOperands.find(opand->getValue()) != m_namedOperands.end() )
  {
      continue; // valid named operand 'Northing', 'Porosity'
  }

  std::string baseTag;
  int timeStep= -1;

  // Is it a name with attached depletion stage? -> aap_D1
  // Allow Porosity_D1 (material item)
  // Do not allow Northing_D1, Displacement_D1
  //
  if (  SimpleSplitTextTag(opand->getValue(),baseTag,timeStep ) && timeStep != -1)
  {
      if ( CFillMaterialParameterProxies::GetNames(m_model).find( baseTag) ==
      CFillMaterialParameterProxies::GetNames(m_model).end()
         )
      {
    std::string msg= 
          "\"" + baseTag + "\" operand does not accept a depletion stage index";
    m_shunt.setMessage( tokens, pos, msg);
    return false;
      }

      if ( m_model.FindDepletionStage( timeStep ) )
      {
    continue; // material item with valid depletion stage
      }
      else
      {
    std::string msg= 
          "\"" + opand->getValue() + "\" material item does not contain a valid depletion stage index";
    m_shunt.setMessage( tokens, pos, msg);
    return false;
      }
  }
  else if (  m_model.ResultTree().ValidName( opand->getValue(),resultInfo )
      || m_model.MeshResultTree().ValidName(opand->getValue(),resultInfo)
      )
  {
      continue;
  }
  else
  {
      std::string msg= 
    "\"" + opand->getValue() + "\" is not a numerical value and not an existing variable.\nPlease only use existing variable names.";
      m_shunt.setMessage( tokens, pos, msg);
      return false;
  }
  } // for tokens

  return true;
}

void CRpnEditFormulaDlg::OnOK()
{
  if ( m_bCaughtException==true )
  {
  CDialog::OnCancel();
  return;
  }

  try
  {
  CString infixFormula;

  GetDlgItem(IDC_EB_RPNFORMULA)->GetWindowText(infixFormula);
  m_sInfixFormula= infixFormula;

  // clear
  while( m_qsRPNFormula.size() > 0 ) m_qsRPNFormula.pop();

  std::list<Shunt::CRpnToken *> tokens;
  Shunt::CRpnOperators *instance= Shunt::CRpnOperators::instance();
  instance->createTokens(m_sInfixFormula, tokens); 

  if ( checkOperandNames( tokens) == true  &&
         m_shunt.convertInfixToPostfix(tokens, m_qsRPNFormula) == true
       )
  {
      m_bValidatedOk= true;
  }
  else
  {
      m_bValidatedOk= false;
      GetDlgItem(IDC_RPN_ERRORMSG)->
    SetWindowText(m_shunt.getMessage().c_str());
  }
  instance->deleteTokens(tokens);

  if ( m_bValidatedOk )
  {
      CDialog::OnOK();
  }
  }
  catch(...)
  {
  CDialog::OnCancel();
  return;
  }
}

void CRpnEditFormulaDlg::DoDataExchange(CDataExchange* pDX)
{
  if ( m_bCaughtException==true ) return;
  try
  {
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CRpnEditFormulaDlg)
  // NOTE: the ClassWizard will add DDX and DDV calls here
  //}}AFX_false

  if(! pDX->m_bSaveAndValidate)
  {
      std::list<Shunt::CRpnToken *> tokens;
      Shunt::CRpnOperators *instance= Shunt::CRpnOperators::instance();
      instance->createTokens(m_sInfixFormula, tokens); 
      instance->deleteTokens(tokens);
      GetDlgItem(IDC_EB_RPNFORMULA)->
      SetWindowText(m_sInfixFormula.c_str());
  }
  }
  catch(...)
  {
  m_bCaughtException= true;
  }
}


BEGIN_MESSAGE_MAP(CRpnEditFormulaDlg, CDialog)
  //{{AFX_MSG_MAP(CRpnEditFormulaDlg)
    // NOTE: the ClassWizard will add message map macros here
  ON_BN_CLICKED(IDOK, OnOK)
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRpnEditFormulaDlg message handlers
