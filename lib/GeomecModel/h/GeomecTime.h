// GeomecTime.h: interface for the CGeomecTime class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GEOMECTIME_H__08267F03_5636_45B5_8CC2_4E5C9BBC6464__INCLUDED_)
#define AFX_GEOMECTIME_H__08267F03_5636_45B5_8CC2_4E5C9BBC6464__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <QDateTime>

class CGeomecTime  
{
public:
	CGeomecTime();
	CGeomecTime(int nYear, int nMonth);
  CGeomecTime(int nYear, int nMonth, int nDay);
  CGeomecTime(const QDateTime& datetime);
	CGeomecTime(const CGeomecTime& rhs);
	virtual ~CGeomecTime();

  CGeomecTime& operator=(const CGeomecTime& rhs);
	bool operator==(const CGeomecTime& rhs) const;
	bool operator!=(const CGeomecTime& rhs) const;
	bool operator<(const CGeomecTime& rhs) const;
	bool operator>(const CGeomecTime& rhs) const;
	bool operator<=(const CGeomecTime& rhs) const;
	bool operator>=(const CGeomecTime& rhs) const;
  operator QDateTime() const;

  long long SecondsTo(const CGeomecTime& rhs) const;
  int DaysTo(const CGeomecTime& rhs) const;
//	double operator-(const CGeomecTime& rhs) const;	// Returns the days between CGeomecTime's
	CGeomecTime operator+(int nDays) const;

	QString Month() const;
	int GetYear() const;
	int GetMonth() const;
  int GetDay() const;
  int GetHour() const;
  int GetMinute() const;
  int GetSecond() const;
	void Set(int nYear, int nMonth);
  void Set(int nYear, int nMonth, int nDay, int nHour, int nMinute, int nSecond);
  void SetOneMonthEarlier();
  void SetOneMonthLater();

private:
  QDateTime m_datetime;
};

#endif // !defined(AFX_GEOMECTIME_H__08267F03_5636_45B5_8CC2_4E5C9BBC6464__INCLUDED_)
