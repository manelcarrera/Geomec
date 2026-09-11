// GeomecTime.cpp: implementation of the CGeomecTime class.
//
//////////////////////////////////////////////////////////////////////

#include "GeomecTime.h"
#include <cassert>

#ifdef _DEBUG
#ifdef _MSC_VER
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif // _MSC_VER
// #define new DEBUG_NEW
#endif

CGeomecTime::CGeomecTime() {}

CGeomecTime::CGeomecTime(int nYear, int nMonth) : m_datetime(QDate(nYear, nMonth, 1)) { assert(m_datetime.isValid()); }

CGeomecTime::CGeomecTime(int nYear, int nMonth, int nDay) : m_datetime(QDate(nYear, nMonth, nDay)) {
  assert(m_datetime.isValid());
}

CGeomecTime::CGeomecTime(const QDateTime &datetime) : m_datetime(datetime) {}

CGeomecTime::CGeomecTime(const CGeomecTime &rhs) : m_datetime(rhs.m_datetime) {}

CGeomecTime::~CGeomecTime() {}

CGeomecTime &CGeomecTime::operator=(const CGeomecTime &rhs) {
  m_datetime = rhs.m_datetime;
  return *this;
}

bool CGeomecTime::operator==(const CGeomecTime &rhs) const { return m_datetime == rhs.m_datetime; }

bool CGeomecTime::operator!=(const CGeomecTime &rhs) const { return !operator==(rhs); }

bool CGeomecTime::operator<(const CGeomecTime &rhs) const { return m_datetime < rhs.m_datetime; }

bool CGeomecTime::operator>(const CGeomecTime &rhs) const { return m_datetime > rhs.m_datetime; }

bool CGeomecTime::operator<=(const CGeomecTime &rhs) const { return m_datetime <= rhs.m_datetime; }
bool CGeomecTime::operator>=(const CGeomecTime &rhs) const { return m_datetime >= rhs.m_datetime; }

CGeomecTime::operator QDateTime() const { return m_datetime; }

long long CGeomecTime::SecondsTo(const CGeomecTime &rhs) const {
  const long long SECS_PER_DAY = 86400;
  long long ll = m_datetime.date().daysTo(rhs.m_datetime.date());
  ll *= SECS_PER_DAY;
  ll += m_datetime.time().secsTo(rhs.m_datetime.time());
  return ll;
}

int CGeomecTime::DaysTo(const CGeomecTime &rhs) const { return m_datetime.daysTo(rhs.m_datetime); }

CGeomecTime CGeomecTime::operator+(int nDays) const {
  CGeomecTime ret(*this);
  ret.m_datetime = ret.m_datetime.addDays(int(nDays));
  return ret;
}

int CGeomecTime::GetYear() const { return m_datetime.date().year(); }

int CGeomecTime::GetMonth() const { return m_datetime.date().month(); }

int CGeomecTime::GetDay() const { return m_datetime.date().day(); }

int CGeomecTime::GetHour() const { return m_datetime.time().hour(); }

int CGeomecTime::GetMinute() const { return m_datetime.time().minute(); }

int CGeomecTime::GetSecond() const { return m_datetime.time().second(); }

void CGeomecTime::Set(int nYear, int nMonth) { m_datetime.setDate(QDate(nYear, nMonth, 1)); }

void CGeomecTime::Set(int nYear, int nMonth, int nDay, int nHour, int nMinute, int nSecond) {
  m_datetime.setDate(QDate(nYear, nMonth, nDay));
  m_datetime.setTime(QTime(nHour, nMinute, nSecond));
}

void CGeomecTime::SetOneMonthEarlier() { m_datetime = m_datetime.addMonths(-1); }

void CGeomecTime::SetOneMonthLater() { m_datetime = m_datetime.addMonths(1); }

QString CGeomecTime::Month() const { return QDate::longMonthName(m_datetime.date().month()); }
