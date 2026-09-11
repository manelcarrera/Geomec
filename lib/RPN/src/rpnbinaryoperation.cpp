// RpnBinaryOperation.cpp: implementation of the CRpnBinaryOperation class.
//
//////////////////////////////////////////////////////////////////////

#include "rpnbinaryoperation.h"
#include <cmath>

namespace rpn {

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRpnBinaryOperation::CRpnBinaryOperation(CRpnStack &stack, enum RPN_OPERATION operation)
: m_operation(operation)
{
	assert(stack.StackSize() > 1);
	m_pOperandB = &stack.Pop();
	m_pOperandA = &stack.Pop();
	stack.Push(*this);
}

CRpnBinaryOperation::CRpnBinaryOperation()
: m_pOperandA(0), m_pOperandB(0)
{
}

CRpnBinaryOperation::~CRpnBinaryOperation()
{
	delete m_pOperandA;
	delete m_pOperandB;
}

CRpnBinaryOperation::TValue CRpnBinaryOperation::Operation(const TValue &value_a, const TValue &value_b) const
{
	if(value_a.Valid() && value_b.Valid())
	{
		switch(m_operation)
		{
		case OP_ADD:
			return  TValue(value_a.Value() + value_b.Value());
		case OP_SUB:
			return  TValue(value_a.Value() - value_b.Value());
		case OP_DIV:
			if(value_b.Value() == 0.0)
				return TValue();	// Error
			return  TValue(value_a.Value() / value_b.Value());
		case OP_MUL:
			return  TValue(value_a.Value() * value_b.Value());
		case OP_EQUAL:
			if(value_a.Value() == value_b.Value())
				return TValue(1.0);	// EQUAL
			return TValue(0.0);		// NOT EQUAL
		case OP_NOT_EQUAL:
			if(value_a.Value() != value_b.Value())
				return TValue(1.0);	// NOT EQUAL
			return TValue(0.0);		// EQUAL
		case OP_LESS:
			if(value_a.Value() < value_b.Value())
				return TValue(1.0);	// LESS
			return TValue(0.0);		// GREATER OR EQUAL
		case OP_LESS_EQUAL:
			if(value_a.Value() <= value_b.Value())
				return TValue(1.0);	// LESS OR EQUAL
			return TValue(0.0);		// GREATER
		case OP_GREATER:
			if(value_a.Value() > value_b.Value())
				return TValue(1.0);	// GREATER
			return TValue(0.0);		// LESS OR EQUAL
		case OP_GREATER_EQUAL:
			if(value_a.Value() >= value_b.Value())
				return TValue(1.0);	// GREATER OR EQUAL
			return TValue(0.0);		// LESS
		case OP_MIN:
			if(value_a.Value() < value_b.Value())
				return value_a;	
			return value_b;	
		case OP_MAX:
			if(value_a.Value() > value_b.Value())
				return value_a;	
			return value_b;	
		case OP_POW:
			return pow(value_a.Value(), value_b.Value());
		case OP_OR:
			if( (fabs(value_a.Value()) > EPS) || (fabs(value_b.Value()) > EPS) )
				return TValue(1);
			return TValue(0);
		case OP_AND:
			if( (fabs(value_a.Value()) > EPS) && (fabs(value_b.Value()) > EPS) )
				return TValue(1);
			return TValue(0);

		default:
				assert(false);
			break;
		}
	}

	return TValue();
}

CRpnBinaryOperation::TValue CRpnBinaryOperation::Value(const geo::IPoint &pt, UNIT unit) const
{
	return Operation(m_pOperandA->Value(pt, unit), m_pOperandB->Value(pt, unit));
}

bool CRpnBinaryOperation::Recursive(TParentSet stParent) const
{
	if(!stParent.insert(this).second)
		return true;
	return m_pOperandA->Recursive(stParent) || m_pOperandB->Recursive(stParent);
}

void CRpnBinaryOperation::CollectUsedObjects(TObjectSet& stObjects) const
{
  stObjects.insert(m_pOperandA);
  m_pOperandA->CollectUsedObjects(stObjects);
  stObjects.insert(m_pOperandB);
  m_pOperandB->CollectUsedObjects(stObjects);
}

CRpnBinaryOperation::TValueVec CRpnBinaryOperation::Value(const geo::IElement &el, UNIT unit) const
{
	// Get value from A and B and size return vector....
	TValueVec A = m_pOperandA->Value(el, unit);
	TValueVec B = m_pOperandB->Value(el, unit);
	TValueVec ret(el.NrOfPoints());
	assert(A.size() == B.size());
	assert(B.size() == ret.size());

	// Do the operation for each coordinate
	for(size_t i = 0; i < ret.size(); i++)
	{
		ret[i] = Operation(A[i], B[i]);
	}
	
	return ret;
}

QString CRpnBinaryOperation::CurrentOperator() const
{
	switch(m_operation)
	{
	case OP_ADD:
		return " + ";
		break;

	case OP_SUB:
		return " - ";
		break;

	case OP_DIV:
		return " / ";
		break;
		
	case OP_MUL:
		return " * ";
		break;
		
	case OP_EQUAL:
		return " = ";
		break;
		
	case OP_NOT_EQUAL:
		return " != ";
		break;

	case OP_LESS:
		return " < ";
		break;
		
	case OP_LESS_EQUAL:
		return " <= ";
		break;
		
	case OP_GREATER:
		return " > ";
		break;
		
	case OP_GREATER_EQUAL:
		return " >= ";
		break;
	case OP_AND:
		return " and ";
		break;
		
	case OP_OR:
		return " or ";
		break;
	default:
		assert(false);	// Not supported
		break;
	}

	return "";
}

// returns the formula
QString CRpnBinaryOperation::Formula() const
{
	QString sRet;

	switch(m_operation)
	{
	case OP_MIN:
		sRet = "min(";
		sRet += m_pOperandA->Formula();
		sRet += ", ";
		sRet += m_pOperandB->Formula();
		sRet += ")";
		break;
	case OP_MAX:
		sRet = "max(";
		sRet += m_pOperandA->Formula();
		sRet += ", ";
		sRet += m_pOperandB->Formula();
		sRet += ")";
		break;
	case OP_POW:
		sRet = "pow(";
		sRet += m_pOperandA->Formula();
		sRet += ", ";
		sRet += m_pOperandB->Formula();
		sRet += ")";
		break;
	default:
		sRet = Operand(*m_pOperandA);
		sRet += CurrentOperator();
		sRet += Operand(*m_pOperandB);
		break;
	}

	return sRet;
}

QString CRpnBinaryOperation::Operand(const CRpnObject &object) const
{
	if(dynamic_cast<const CRpnBinaryOperation*>(&object))
	{
		QString sRet = "(";
		sRet += object.Formula();
		sRet += ")";
		return sRet;
	}	

	return object.Formula();
}



void CRpnBinaryOperation::Clear(CRpnStack &stack)
{
	assert(&stack.Top() == this);
	stack.Pop();
	stack.Push(*m_pOperandA);
	stack.Push(*m_pOperandB);
	m_pOperandA = 0;
	m_pOperandB = 0;
	delete this;
}

CRpnBinaryOperation::eObjectType CRpnBinaryOperation::ObjectType() const
{
	return OT_BINARY;
}

CRpnObject* CRpnBinaryOperation::Clone(CRpnStack& NewStack) const
{
	CRpnBinaryOperation *pClone = new CRpnBinaryOperation(*this);	
	pClone->m_pOperandA = m_pOperandA->Clone(NewStack);
	pClone->m_pOperandB = m_pOperandB->Clone(NewStack);
	pClone->m_operation = m_operation;
	assert(pClone->m_pOperandA && pClone->m_pOperandB);
	return pClone;
}

void CRpnBinaryOperation::SaveStream(std::stringstream& stream)
{
	// Let the base class do it's work
	CRpnObject::SaveStream(stream);

	// Save operands
	m_pOperandA->SaveStream(stream);
	m_pOperandB->SaveStream(stream);

	// Save the operator tag
	int nOperator = m_operation;
	stream << nOperator << " ";
}
	
void CRpnBinaryOperation::LoadStream(std::stringstream& stream, CStreamVersion& version, CRpnStack& stack)
{
	// Let the base class do it's work
	CRpnObject::LoadStream(stream, version, stack);

	// Load the objects
	m_pOperandA = LoadRpnObject(stream, version, stack);
	m_pOperandB = LoadRpnObject(stream, version, stack);

	// Load the operator
	int nOperator;
	stream >> nOperator;
	m_operation = RPN_OPERATION(nOperator);
}

bool CRpnBinaryOperation::exists() const
{
  assert(m_pOperandA->exists() && m_pOperandB->exists());

  return (m_pOperandA->exists() && m_pOperandB->exists());
}

CRpnObject* CRpnBinaryOperation::GetExpandedRpnObject(CRpnStack& targetstack, std::list<std::string>& lstMessages) const
{
  CRpnBinaryOperation* pCopy = new CRpnBinaryOperation();
  pCopy->m_operation = m_operation;
  pCopy->m_pOperandA = m_pOperandA->GetExpandedRpnObject(targetstack, lstMessages);
  pCopy->m_pOperandB = m_pOperandB->GetExpandedRpnObject(targetstack, lstMessages);

  return pCopy;
}

} // namespace rpn
