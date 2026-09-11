 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */
// Value.cpp: implementation of the CValue class.
//
//////////////////////////////////////////////////////////////////////

#include <cmath>
#include <limits>

#include "dimple.h"
//#include <sstream>
#include <cassert>
#include "Value.h"

#ifdef WIN32
# define isinf(v) (! _finite(v))
# define isnan(v) _isnan(v)
#else
# define isnan(v) std::isnan(v)
# define isinf(v) std::isinf(v)
#endif

#ifdef _DEBUG
# define M_BVALID(b) m_bValid= (b)
#else
# define M_BVALID(b) 
#endif

namespace geo {
static inline bool _isvalid(const double &v)
{
  return !(isnan(v) || isinf(v));
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CValue::CValue()
: m_dValue(std::numeric_limits<double>::quiet_NaN())
{
	M_BVALID(false);
}

CValue::CValue(const double &dValue)
: m_dValue(std::numeric_limits<double>::quiet_NaN())
{
	M_BVALID( _isvalid( dValue));
	if (_isvalid( dValue))
		m_dValue= dValue;
}

CValue::CValue(const IValue &value)
: m_dValue(std::numeric_limits<double>::quiet_NaN())
{
	M_BVALID(value.Valid());
	if(value.Valid())
		m_dValue = value.Value();
}

bool CValue::Valid() const
{
	return _isvalid(m_dValue);
}

void CValue::Invalidate()
{
  M_BVALID(false);
  m_dValue = std::numeric_limits<double>::quiet_NaN();
}

const double &CValue::Value() const
{
	assert(Valid());
	return m_dValue;
}

void CValue::Value(const double &dValue)
{
	if ( _isvalid(dValue) )
	{
		m_dValue = dValue;
		M_BVALID(true);
	}
	else
	{
		m_dValue= std::numeric_limits<double>::quiet_NaN();
		M_BVALID(false);
	}
}

CValue &CValue::operator=(const double &dValue)
{
	Value(dValue);

	return *this;
}

CValue& CValue::operator=(const IValue &value)
{
	Value( value.Value());

	return *this;
}

std::string CValue::Representation() const
{
	if(!Valid()) return std::string("NaN");

    char buf[1024];
    snprintf(buf, 1023, "%f", Value());

    return std::string(buf);
}

}
