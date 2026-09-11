// RpnUnaryOperation.cpp: implementation of the CRpnUnaryOperation class.
//
//////////////////////////////////////////////////////////////////////

#include "rpnunaryoperation.h"
#include <cmath>

namespace rpn {
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CRpnUnaryOperation::CRpnUnaryOperation()
: m_pOperand(0)
{
}

CRpnUnaryOperation::CRpnUnaryOperation(CRpnStack &stack, enum CRpnUnaryOperation::RPN_OPERATION operation)
: m_operation(operation)
{
	assert(stack.StackSize() > 0);
	m_pOperand = &stack.Top();
	stack.Pop();
	stack.Push(*this);
}

CRpnUnaryOperation::~CRpnUnaryOperation()
{
	delete m_pOperand;
}

CRpnUnaryOperation::TValue CRpnUnaryOperation::Operation(const TValue &value) const
{
	if(value.Valid())
	{
		switch(m_operation)
		{
		case OP_SQRT:
			if(value.Value() < 0)
				return TValue();
			return TValue(sqrt(value.Value()));
		case OP_SIN:
			return TValue(sin(value.Value()));
		case OP_COS:
			return TValue(cos(value.Value()));
		case OP_TAN:
			return TValue(tan(value.Value()));
		case OP_LOG:
			return TValue(log10(value.Value()));
		case OP_LN:
			return TValue(log(value.Value()));
		case OP_EXP:
			return TValue(exp(value.Value()));
		case OP_ABS:
			return TValue(abs(value.Value()));
		case OP_NOT:
			if(abs(value.Value()) < EPS)
				return TValue(1);
			else
				return TValue(0);
		case OP_SINH:
			return TValue(sinh(value.Value()));
		case OP_COSH:
			return TValue(cosh(value.Value()));
		case OP_TANH:
			return TValue(tanh(value.Value()));
		case OP_SIGN: 
			if(value.Value() < 0)
				return TValue(-1);
			if(value.Value() > 0)
				return TValue(1);
			return TValue(0);
		case OP_ROUND:
			return TValue(floor(value.Value() + 0.5));
		case OP_ASIN:
			if(value.Value() >= -1.0 && value.Value() <= 1.0) return TValue(asin(value.Value()));
			else return TValue();
		case OP_ACOS:
			if(value.Value() >= -1.0 && value.Value() <= 1.0) return TValue(acos(value.Value()));
			else return TValue();
		case OP_ATAN:
			return TValue(atan(value.Value()));
		default:
			assert(false);
			break;
		}
	}

	return TValue();
}

CRpnUnaryOperation::TValue CRpnUnaryOperation::Value(const geo::IPoint &pt, UNIT unit) const
{
	return Operation(m_pOperand->Value(pt, unit));
}

CRpnUnaryOperation::TValueVec CRpnUnaryOperation::Value(const geo::IElement &el, UNIT unit) const
{
	// Fetch operand and size the return vector
	TValueVec A = m_pOperand->Value(el, unit);
	TValueVec ret(el.NrOfPoints());
	assert(A.size() == ret.size());

	// Do formula for each points
	for(size_t i = 0; i < ret.size(); i++)
	{
		ret[i] = Operation(A[i]);
	}

	return ret;
}

QString CRpnUnaryOperation::Formula() const
{
	QString sRet;
	switch(m_operation)
	{
	case OP_SQRT:
		sRet = "sqrt(";
		sRet += m_pOperand->Formula();
		sRet += ")";
		break;
	case OP_SIN:
		sRet = "sin(";
		sRet += m_pOperand->Formula();
		sRet += ")";
		break;
	case OP_COS:
		sRet = "cos(";
		sRet += m_pOperand->Formula();
		sRet += ")";
		break;
	case OP_TAN:
		sRet = "tan(";
		sRet += m_pOperand->Formula();
		sRet += ")";
		break;
	case OP_ASIN:
		sRet = "arcsin(";
		sRet += m_pOperand->Formula();
		sRet += ")";
		break;
	case OP_ACOS:
		sRet = "arccos(";
		sRet += m_pOperand->Formula();
		sRet += ")";
		break;
	case OP_ATAN:
		sRet = "arctan(";
		sRet += m_pOperand->Formula();
		sRet += ")";
		break;
	case OP_LOG:
		sRet = "log(";
		sRet += m_pOperand->Formula();
		sRet += ")";
		break;
	case OP_LN:
		sRet = "ln(";
		sRet += m_pOperand->Formula();
		sRet += ")";
		break;
	case OP_ABS:
		sRet = "abs(";
		sRet += m_pOperand->Formula();
		sRet += ")";
		break;
	case OP_EXP:
		sRet = "exp(";
		sRet += m_pOperand->Formula();
		sRet += ")";
		break;
	case OP_NOT:
		sRet = "not(";
		sRet += m_pOperand->Formula();
		sRet += ")";
		break;
	case OP_SINH:
		sRet = "sinh(";
		sRet += m_pOperand->Formula();
		sRet += ")";
		break;
	case OP_COSH:
		sRet = "cosh(";
		sRet += m_pOperand->Formula();
		sRet += ")";
		break;
	case OP_TANH:
		sRet = "tanh(";
		sRet += m_pOperand->Formula();
		sRet += ")";
		break;
	case OP_SIGN: 
		sRet = "sign(";
		sRet += m_pOperand->Formula();
		sRet += ")";
		break;
	case OP_ROUND:
		sRet = "round(";
		sRet += m_pOperand->Formula();
		sRet += ")";
		break;
	default:
		assert(false);
		break;
	}

	return sRet;
}

void CRpnUnaryOperation::Clear(CRpnStack &stack)
{
	assert(&stack.Top() == this);
	stack.Pop();
	stack.Push(*m_pOperand);
	m_pOperand = 0;
	delete this;
}

void CRpnUnaryOperation::SaveStream(std::stringstream& stream)
{
	CRpnObject::SaveStream(stream);

	m_pOperand->SaveStream(stream);

	int nOperation = m_operation;
	stream << nOperation << " ";
}

void CRpnUnaryOperation::LoadStream(std::stringstream& stream, CStreamVersion& version, CRpnStack& stack)
{
	CRpnObject::LoadStream(stream, version, stack);

	m_pOperand = LoadRpnObject(stream, version, stack);

	int nOperation;
	stream >> nOperation;
	m_operation = (RPN_OPERATION)nOperation;
}

bool CRpnUnaryOperation::Recursive(TParentSet stParent) const
{
	if(!stParent.insert(this).second)
		return true;
	return m_pOperand->Recursive(stParent);
}

void CRpnUnaryOperation::CollectUsedObjects(TObjectSet& stObjects) const
{
  stObjects.insert(m_pOperand); 
  m_pOperand->CollectUsedObjects(stObjects);
}

CRpnUnaryOperation::eObjectType CRpnUnaryOperation::ObjectType() const
{
	return OT_UNARY;
}

CRpnObject* CRpnUnaryOperation::Clone(CRpnStack& NewStack) const
{
	CRpnUnaryOperation *pClone = new CRpnUnaryOperation(*this);
	pClone->m_operation = m_operation;
	pClone->m_pOperand = m_pOperand->Clone(NewStack);
	return pClone;
}

bool CRpnUnaryOperation::exists() const
{
  assert(m_pOperand->exists());

  return m_pOperand->exists();
}

CRpnObject* CRpnUnaryOperation::GetExpandedRpnObject(CRpnStack& targetstack, std::list<std::string>& lstMessages) const
{
  CRpnUnaryOperation* pCopy = new CRpnUnaryOperation();

  pCopy->m_operation = m_operation;
  pCopy->m_pOperand = m_pOperand->GetExpandedRpnObject(targetstack, lstMessages);

  return pCopy;
}

}
