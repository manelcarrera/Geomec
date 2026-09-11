 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */
#include "FvProperty.h"
#include <cassert>

CProperty::CProperty(const QString& name)
: m_strName(name), m_nSize(1)
{
}

CProperty::~CProperty()
{

}

int CProperty::Size() const
{
	return m_nSize;
}

void CProperty::Size(int nSize)
{
	assert(m_mpValue.size() == 0);
	assert(nSize > 0);
	m_nSize = nSize;
}

const QString& CProperty::Name() const
{
	return m_strName;
}

const QString& CProperty::UnitName() const
{
	return m_strUnitName;
}

const QString& CProperty::ClassName() const
{
	return m_strClassName;
}

void CProperty::UnitName(const QString &unit_name)
{
	m_strUnitName = unit_name;
}

void CProperty::ClassName(const QString &class_name)
{
	m_strClassName = class_name;
}

const CProperty::TPropertyVec& CProperty::Value(const geo::IPoint& point) const
{
	TPointMap::const_iterator it = m_mpValue.find(&point);

	if(it != m_mpValue.end())
		return it->second;

	return m_vcEmpty;
}

void CProperty::AddValue(const geo::IPoint& point, const CProperty::TPropertyVec &property)
{
	m_mpValue.insert(TPointMap::value_type(&point, property));
}
