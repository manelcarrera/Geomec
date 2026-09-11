// ProgressCallBack.h: interface for the CProgressCallBack class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PROGRESSCALLBACK_H__6F6CBA83_D489_49BD_AD2A_16931C96D33B__INCLUDED_)
#define AFX_PROGRESSCALLBACK_H__6F6CBA83_D489_49BD_AD2A_16931C96D33B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

namespace gm3 {
//##ModelId=3BA8A1C802A4
class CProgressCallBack  
{
  //##ModelId=3BA8A1C802A5
  long m_lTotalRecords;
  //##ModelId=3BA8A1C802B3
  long m_lRecordCount;
public:
  int CurrentRecord() const;
  //##ModelId=3BA8A1C802B4
  CProgressCallBack();
  //##ModelId=3BA8A1C802B5
  void TotalRecords(const long lTotalRecords);
  //##ModelId=3BA8A1C802B7
  long TotalRecords() const;
  //##ModelId=3BA8A1C802B9
  virtual void OnNewRecord();
  //##ModelId=3BA8A1C802C4
  virtual void OnNewTable(const CString &strTableName);
  //##ModelId=3BA8A1C802C7
  virtual void OnCancel();
};
}

#endif // !defined(AFX_PROGRESSCALLBACK_H__6F6CBA83_D489_49BD_AD2A_16931C96D33B__INCLUDED_)
