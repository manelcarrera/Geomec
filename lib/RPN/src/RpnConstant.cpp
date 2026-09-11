// RpnConstant.cpp: implementation of the CRpnConstant class.
//
//////////////////////////////////////////////////////////////////////
#include <cmath>
#include "rpnstack.h"
#include "RpnConstant.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
namespace rpn {

CRpnConstant::CRpnConstant()
{

}

CRpnConstant::CRpnConstant(CRpnStack& stack, RPN_CONSTANT constant)
: m_constant(constant)
{
	stack.Push(*this);
}

CRpnConstant::~CRpnConstant()
{

}

CRpnConstant::TValue CRpnConstant::Value(const geo::IPoint &/*pt*/, UNIT /*unit*/) const
{
	switch(m_constant)
	{
	case CON_PI:
		return TValue(PI);
	case CON_E:
		return TValue(exp((double)1));
	};

	assert(false);
	return 0;
}

CRpnConstant::TValueVec CRpnConstant::Value(const geo::IElement &el, UNIT unit) const
{
	// Fetch operand and size the return vector
	TValueVec ret(el.NrOfPoints());

	// Do formula for each points
	for(int i = 0; i < ret.size(); i++)
	{
		ret[i] = Value(el.Point(i), unit);
	}

	return ret;
}

QString CRpnConstant::Formula() const
{
	switch(m_constant)
	{
	case CON_PI:
		return "pi";
	case CON_E:
		return "e";
	};

	assert(false);
	return "";
}

void CRpnConstant::Clear(CRpnStack &stack)
{
	assert(&stack.Top() == this);
	stack.Pop();
	delete this;
}

void CRpnConstant::SaveStream(std::stringstream& stream)
{
	CRpnObject::SaveStream(stream);

	int nConstant = m_constant;
	stream << nConstant << " ";
}

void CRpnConstant::LoadStream(std::stringstream& stream, CStreamVersion& version, CRpnStack& stack)
{
	CRpnObject::LoadStream(stream, version, stack);

	int nConstant;
	stream >> nConstant;
	m_constant = (RPN_CONSTANT)nConstant;
}

CRpnConstant::eObjectType CRpnConstant::ObjectType() const
{
	return OT_RPN_CONSTANT;
}

CRpnObject* CRpnConstant::Clone(CRpnStack& /*NewStack*/) const
{
	CRpnConstant *pClone = new CRpnConstant(*this);
	pClone->m_constant = m_constant;
	return pClone;
}

bool CRpnConstant::Recursive(TParentSet /*stParent*/) const
{
	return false;
}

bool CRpnConstant::exists() const
{
  return true;
}

CRpnObject* CRpnConstant::GetExpandedRpnObject(CRpnStack& targetstack, std::list<std::string>& /*lstMessages*/) const
{
  return Clone(targetstack);
}

}
