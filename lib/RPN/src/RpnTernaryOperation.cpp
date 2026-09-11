// RpnTernaryOperation.cpp: implementation of the CRpnTernaryOperation class.
//
//////////////////////////////////////////////////////////////////////

#include <cmath>
#include "RpnTernaryOperation.h"
#include "rpnstack.h"

namespace rpn{ 
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRpnTernaryOperation::CRpnTernaryOperation()
: m_pOperandA(0), m_pOperandB(0), m_pOperandC(0)
{

}

CRpnTernaryOperation::CRpnTernaryOperation(CRpnStack& stack, RPN_OPERATION operation)
: m_operation(operation)
{
  assert(stack.StackSize() > 2);
  
  m_pOperandC = &stack.Pop();
  m_pOperandB = &stack.Pop();
  m_pOperandA = &stack.Pop();

  stack.Push(*this);
}

CRpnTernaryOperation::~CRpnTernaryOperation()
{
  delete m_pOperandA;
  delete m_pOperandB;
  delete m_pOperandC;
}

CRpnTernaryOperation::TValue CRpnTernaryOperation::Operation(const TValue &value_a, const TValue &value_b, const TValue &value_c) const
{
  assert(m_operation == OP_IF);
  // We only can do the if clause
  if(value_a.Valid())
  {
    // When zero 0 return the value of operand C, otherwise use operand B
    if(fabs(value_a.Value()) < EPS)
      return value_c;
    return value_b;
  }

  // Invalid
  return TValue();
}

CRpnTernaryOperation::TValue CRpnTernaryOperation::Value(const geo::IPoint &pt, UNIT unit) const
{
  return Operation(m_pOperandA->Value(pt, unit), m_pOperandB->Value(pt, unit), m_pOperandC->Value(pt, unit));
}

CRpnTernaryOperation::TValueVec CRpnTernaryOperation::Value(const geo::IElement &el, UNIT unit) const
{
  // Fetch a, b, c and size ret
  TValueVec A = m_pOperandA->Value(el, unit);
  TValueVec B = m_pOperandB->Value(el, unit);
  TValueVec C = m_pOperandC->Value(el, unit);
  TValueVec ret(el.NrOfPoints());
  assert(A.size() == B.size());
  assert(B.size() == C.size());
  assert(C.size() == ret.size());

  // Do the formula for all points
  for(size_t i = 0; i < ret.size(); i++)
  {
    ret[i] = Operation(A[i], B[i], C[i]);
  }

  return ret;
}


QString CRpnTernaryOperation::Formula() const
{
  QString sRet;
  sRet +="if(";
  sRet += m_pOperandA->Formula();
  sRet += ") then(";
  sRet += m_pOperandB->Formula();
  sRet += ") else(";
  sRet += m_pOperandC->Formula();
  sRet += ")";
  return sRet;
}

void CRpnTernaryOperation::Clear(CRpnStack &stack)
{
  assert(&stack.Top() == this);
  stack.Pop();
  stack.Push(*m_pOperandA);
  stack.Push(*m_pOperandB);
  stack.Push(*m_pOperandC);
  m_pOperandA = 0;
  m_pOperandB = 0;
  m_pOperandC = 0;
  delete this;
}

CRpnTernaryOperation::eObjectType CRpnTernaryOperation::ObjectType() const
{
  return OT_TERNARY;
}

CRpnObject* CRpnTernaryOperation::Clone(CRpnStack& NewStack) const
{
  CRpnTernaryOperation *pClone = new CRpnTernaryOperation();
  pClone->m_operation = m_operation;
  pClone->m_pOperandA = m_pOperandA->Clone(NewStack);
  pClone->m_pOperandB = m_pOperandB->Clone(NewStack);
  pClone->m_pOperandC = m_pOperandC->Clone(NewStack);

  return pClone;
}

// Save functions
void CRpnTernaryOperation::SaveStream(std::stringstream& stream)
{
  // Save base first
  CRpnObject::SaveStream(stream);

  // Save the operands
  m_pOperandA->SaveStream(stream);
  m_pOperandB->SaveStream(stream);
  m_pOperandC->SaveStream(stream);

  // Save operand
  int nOperand = (int)m_operation;
  stream << nOperand << " ";
}

bool CRpnTernaryOperation::Recursive(TParentSet stParent) const
{
  if(!stParent.insert(this).second)
    return true;
  return m_pOperandA->Recursive(stParent) ||
       m_pOperandB->Recursive(stParent) ||
       m_pOperandC->Recursive(stParent);
}

void CRpnTernaryOperation::CollectUsedObjects(TObjectSet& stObjects) const
{
  stObjects.insert(m_pOperandA);
  m_pOperandA->CollectUsedObjects(stObjects);
  stObjects.insert(m_pOperandB);
  m_pOperandB->CollectUsedObjects(stObjects);
  stObjects.insert(m_pOperandC);
  m_pOperandC->CollectUsedObjects(stObjects);
}

void CRpnTernaryOperation::LoadStream(std::stringstream& stream, CStreamVersion& version, CRpnStack& stack)
{
  // Load base first
  CRpnObject::LoadStream(stream, version, stack);

  // Save the operands
  m_pOperandA = LoadRpnObject(stream, version, stack);
  m_pOperandB = LoadRpnObject(stream, version, stack);
  m_pOperandC = LoadRpnObject(stream, version, stack);

  // Save operand
  int nOperand;
  stream >> nOperand;
  m_operation = (RPN_OPERATION)nOperand;
}

bool CRpnTernaryOperation::exists() const
{
  assert(m_pOperandA->exists() && m_pOperandB->exists() &&
  m_pOperandC->exists());

  return (m_pOperandA->exists() && m_pOperandB->exists() &&
  m_pOperandC->exists());
}

CRpnObject* CRpnTernaryOperation::GetExpandedRpnObject(CRpnStack& targetstack, std::list<std::string>& lstMessages) const
{
  CRpnTernaryOperation* pCopy = new CRpnTernaryOperation();
  pCopy->m_operation = m_operation;

  pCopy->m_pOperandA = m_pOperandA->GetExpandedRpnObject(targetstack, lstMessages);
  pCopy->m_pOperandB = m_pOperandB->GetExpandedRpnObject(targetstack, lstMessages);
  pCopy->m_pOperandC = m_pOperandC->GetExpandedRpnObject(targetstack, lstMessages);

  return pCopy;
}

}
