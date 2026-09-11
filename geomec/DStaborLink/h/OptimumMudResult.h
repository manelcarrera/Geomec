// OptimumMudResult.h: interface for the COptimumMudResult class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OPTIMUMMUDRESULT_H__B7286988_0427_445B_8ADC_BE97A0DDB132__INCLUDED_)
#define AFX_OPTIMUMMUDRESULT_H__B7286988_0427_445B_8ADC_BE97A0DDB132__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000



class CResultValue  : public geo::CValue
{
  int m_iStatus;
public:
  //get the label for de status of a resultvalue
  static CString GetDsbCaseString(int iDsbCase);

  CResultValue()
  :m_iStatus(-1)
  {} 
  virtual ~CResultValue(){}
  bool IsCalculated();
  void Invalidate()
  {
    m_iStatus=-1;
  }
  CString Info();

  int Status() const 
  {
    return m_iStatus;
  }

  void Status(int status)
  {
    m_iStatus=status;
  }

};












#endif // !defined(AFX_OPTIMUMMUDRESULT_H__B7286988_0427_445B_8ADC_BE97A0DDB132__INCLUDED_)
