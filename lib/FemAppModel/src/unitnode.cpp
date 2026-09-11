// UnitNode.cpp: implementation of the CUnitNode class.
//
//////////////////////////////////////////////////////////////////////

#include "unitnode.h"
#include "ISettings.h"

#ifdef _DEBUG
#ifdef _MSC_VER
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#endif  // _MSC_VER
//#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


CUnitNode::CUnitNode(TUnitType unit)
: CGraphNode(""), m_unit(unit)
{
	ReadRegister();
  Name(UnitName(m_unit));
}

CUnitNode::TUnitType CUnitNode::Unit() const
{
	return m_unit;
}

void CUnitNode::Unit(TUnitType unit)
{
	assert((unit == IQuantityDouble::SI_UNIT) || (unit == IQuantityDouble::FIELD_UNIT));
	m_unit = unit;
	Name(UnitName(unit));
	StoreRegister();
	Modified();
}

QString CUnitNode::UnitName(TUnitType unit) const
{
	QString strRet;
	switch(unit)
	{
	case IQuantityDouble::SI_UNIT:
		return "SI";
		break;
	case IQuantityDouble::FIELD_UNIT:
		return "FIELD";
		break;
	default:
		assert(false);	// Type not defined 
		break;
	}

	return strRet;
}

unsigned int CUnitNode::IconId() const
{
	return 0;
}

unsigned int CUnitNode::TypeId() const
{
	return 0;
}

void CUnitNode::StoreRegister()
{
	ISettings* registry = ISettings::instance();
	int nUnit = (int)(m_unit);
	registry->writeProfileInt("Settings", "UNIT", nUnit);
}

void CUnitNode::ReadRegister()
{
	ISettings* registry = ISettings::instance();
	int nUnit = registry->getProfileInt("Settings", "UNIT", -1);
	switch(nUnit)
	{
	case IQuantityDouble::FIELD_UNIT:
		m_unit = IQuantityDouble::FIELD_UNIT;
		break;
	case IQuantityDouble::US_UNIT:
		m_unit = IQuantityDouble::US_UNIT;
		break;
	case IQuantityDouble::SI_USER_UNIT:
		m_unit = IQuantityDouble::SI_USER_UNIT;
		break;
	case IQuantityDouble::SI_UNIT:
	default:
		m_unit = IQuantityDouble::SI_UNIT;
		break;
	};
}