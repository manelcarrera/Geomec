// DCasingPointResultData.h: interface for the DCasingPointResult struct.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DCASINGPOINTRESULTDATA_H__FB5AD330_814A_4675_8DA1_1B600C883695__INCLUDED_)
#define AFX_DCASINGPOINTRESULTDATA_H__FB5AD330_814A_4675_8DA1_1B600C883695__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <qstring.h>

#ifdef _WIN32
#define MAKESTRING2(str)  #str
#define MAKESTRING(str) MAKESTRING2(str)
#pragma message("WARNING: disable C4718 (deleting code in Qt) in " __FILE__ "[" MAKESTRING(__LINE__) "]")
#pragma warning(push)
#pragma warning(disable: 4718)
#endif

#include <qmap.h>

#ifdef _WIN32
#pragma warning(pop)
#endif

struct DCasingPointSupportedResult  
{
  double m_LB;
  double m_EPS1buck;
  double m_EPS1loss_of_clear;
  int m_Status;

  DCasingPointSupportedResult()
  {
    m_LB = 0;
    m_EPS1buck = 0;
    m_EPS1loss_of_clear = 0;
    m_Status = -1;
  }
};

typedef QMap<short,DCasingPointSupportedResult> DCasingPointSupportedResultMap;

struct DCasingPointUnsupportedResult  
{
  double m_LB1;
  double m_LB2;
  int m_Status;


  DCasingPointUnsupportedResult()
  {
    m_LB1 = 0;
    m_LB2 = 0;
    m_Status = -1;
  }
};

typedef QMap<short,DCasingPointUnsupportedResult> DCasingPointUnsupportedResultMap;

#endif // !defined(AFX_DCASINGPOINTRESULTDATA_H__FB5AD330_814A_4675_8DA1_1B600C883695__INCLUDED_)
