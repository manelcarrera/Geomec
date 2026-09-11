// StatusValue.cpp: implementation of the CStatusValue class.
//
//////////////////////////////////////////////////////////////////////

#include <sstream>

#include <qstring.h>

#include "StatusValue.h"
#include "GeomecStringTable.h"
#include "resourceIDS.h"

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



CDsbStatusValue::CDsbStatusValue()
:m_status(NOT_CALCULATED)
{

}

bool CDsbStatusValue::Valid() const
{
  return IsCalculated();
}

const double &CDsbStatusValue::Value() const
{
  assert(Valid());
  return m_dValue;
}

bool CDsbStatusValue::IsCalculated() const 
{
  return m_status != NOT_CALCULATED;
}

void CDsbStatusValue::Invalidate()
{
  m_status = NOT_CALCULATED;
}

CDsbStatusValue::eDsbStatus CDsbStatusValue::Status() const
{
  return m_status;
}

void CDsbStatusValue::Value(const double& value, eDsbStatus status)
{
  m_status = status;
  m_dValue = value;
}

std::string CDsbStatusValue::Representation() const
{
  if(!Valid()) return std::string("NAN");

  std::stringstream ss;
  ss << Value();
  return ss.str();
}

QString CDsbStatusValue::StatusText() const
{
  QString sRet;
  switch(m_status)
  {
  case NOT_CALCULATED:
    sRet = getStringTableEntry(IDS_DSB_NOT_CALCULATED);
    break;
  case UNKNOWN_ERROR:
    sRet = getStringTableEntry(IDS_DSB_UNKNOWN_ERROR);
    break;
  case OPTIMUM_MUDWEIGHT_FOUND:
    sRet = getStringTableEntry(IDS_DSB_OPTIMUM_MUDWEIGHT_FOUND);
    break;
  case WELL_ALWAYS_STABLE:
    sRet = getStringTableEntry(IDS_DSB_WELL_ALWAYS_STABLE);
    break;
  case PREMATURE_ANALYSIS_FAILURE:
    sRet = getStringTableEntry(IDS_DSB_PREMATURE_ANALYSIS_FAILURE);
    break;
  case WELL_ALWAYS_UNSTABLE:
    sRet = getStringTableEntry(IDS_DSB_WELL_ALWAYS_UNSTABLE);
    break;
  case UNABLE_DETERMINE_STABLE_MUDWEIGHT:
    sRet = getStringTableEntry(IDS_DSB_UNABLE_DETERMINE_STABLE_MUDWEIGHT);
    break;
  default:
    assert(false);
    sRet = getStringTableEntry(IDS_DSB_UNKNOWN_ERROR);
    break;
  }

  return sRet;
}