// ConstantOperand.cpp: implementation of the CRpnConstantOperand class.
//
//////////////////////////////////////////////////////////////////////

#include "rpnconstantoperand.h"

namespace rpn {

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


CRpnConstantOperand::CRpnConstantOperand()
{

}

CRpnConstantOperand::~CRpnConstantOperand()
{
}

void CRpnConstantOperand::Zero()
{
  if(m_sNumber.compare("0", Qt::CaseInsensitive) != 0)
    m_sNumber += "0";
}

void CRpnConstantOperand::One()
{
  m_sNumber += "1";
}

void CRpnConstantOperand::Two()
{
  m_sNumber += "2";
}

void CRpnConstantOperand::Three()
{
  m_sNumber += "3";
}

void CRpnConstantOperand::Four()
{
  m_sNumber += "4";
}

void CRpnConstantOperand::Five()
{
  m_sNumber += "5";
}

void CRpnConstantOperand::Six()
{
  m_sNumber += "6";
}

void CRpnConstantOperand::Seven()
{
  m_sNumber += "7";
}

void CRpnConstantOperand::Eight()
{
  m_sNumber += "8";
}

void CRpnConstantOperand::Nine()
{
  m_sNumber += "9";
}

void CRpnConstantOperand::Point()
{
  if(m_sNumber.indexOf('.') == -1)
  {
    if(m_sNumber.length() == 0)
      m_sNumber = "0";

    m_sNumber += ".";
  }
}

void CRpnConstantOperand::Scn()
{
  if(m_sNumber.indexOf('e') == -1)
  {
    if(m_sNumber.length() == 0)
      m_sNumber = "0";

    m_sNumber += "e";
  }
}

void CRpnConstantOperand::Sign()
{
  bool bSignExponent= false;
  int nExponent = m_sNumber.indexOf('e');

  // wjrx mantis 3184
  //
  if ( m_sNumber.length() >= 2
    && nExponent == m_sNumber.length()-2
    && m_sNumber.right(1) == "-" )
  {
    bSignExponent= true; // 42e-
  }
  else if ( nExponent == m_sNumber.length()-1 )
  {
    bSignExponent= true; // 42e
  }

  if ( bSignExponent == false ) // 42e1 <-> -42e1
  {
    if(m_sNumber.left(1) == "-")
      m_sNumber = m_sNumber.right(m_sNumber.length() - 1);
    else
      m_sNumber = "-" + m_sNumber;
  }
  else // 42e <-> 42e-
  {
    QString sExponent = m_sNumber.right(m_sNumber.length() - nExponent - 1);
    if(sExponent.length() == 0)
      m_sNumber += "-";
    else
    {
      if(sExponent.left(1) == "-")
        sExponent = sExponent.right(sExponent.length() - 1);
      else
        sExponent = "-" + sExponent;
      m_sNumber = m_sNumber.left(nExponent + 1) + sExponent;
      
    }
  }
}

CRpnConstantOperand::TValue CRpnConstantOperand::Value(const geo::IPoint &/*pt*/, UNIT /*unit*/) const
{
  return TValue(m_sNumber.toDouble());
}

CRpnConstantOperand::TValueVec CRpnConstantOperand::Value(const geo::IElement &el, UNIT /*unit*/) const
{
  // Get the number and assign to the vector
  TValue value = TValue(m_sNumber.toDouble());

  TValueVec vcRet(el.NrOfPoints());
  for(size_t i = 0; i < vcRet.size(); i++)
  {
    vcRet[i] = value;
  }

  return vcRet;
}

QString CRpnConstantOperand::Formula() const
{
  return m_sNumber;
}
  
void CRpnConstantOperand::Clear(CRpnStack &stack)
{
  assert(&stack.Top() == this);
  assert(m_sNumber.length() != 0);
  if(m_sNumber.length() == 1)
  {
    stack.Pop();
    delete this;
  }
  else
    m_sNumber.remove(m_sNumber.length() - 1, 1);
}
  
CRpnConstantOperand::eObjectType CRpnConstantOperand::ObjectType() const
{
  return OT_CONSTANT;
}

CRpnObject* CRpnConstantOperand::Clone(CRpnStack& /*NewStack*/) const
{
  CRpnConstantOperand *pClone = new CRpnConstantOperand();
  pClone->m_sNumber = m_sNumber;
  return pClone;
}

void CRpnConstantOperand::SaveStream(std::stringstream& stream)
{
  // Save the base ...
  CRpnObject::SaveStream(stream);

  // Save the number ...
  stream << m_sNumber.toStdString() << " ";
}
  
void CRpnConstantOperand::LoadStream(std::stringstream& stream, CStreamVersion& version, CRpnStack& stack)
{
  // Load the base
  CRpnObject::LoadStream(stream, version, stack);

  // Load the number
  std::string sTemp;
  stream >> sTemp;
  m_sNumber = sTemp.c_str();
}

bool CRpnConstantOperand::Recursive(TParentSet /*stParent*/) const
{
  return false;
}

bool CRpnConstantOperand::exists() const
{
  return true;
}

CRpnObject* CRpnConstantOperand::GetExpandedRpnObject(CRpnStack& /*targetstack*/, std::list<std::string>& /*lstMessages*/) const
{
  CRpnConstantOperand* pCopy = new CRpnConstantOperand();
  pCopy->m_sNumber = m_sNumber;

  return pCopy;
}

}
