// ProgressCallBack.cpp: implementation of the CProgressCallBack class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#ifdef _MSC_VER
#pragma warning ( disable : 4786 )
#endif  // _MSC_VER
#include "GM3ProgressCallBack.h"
#include "gm3CancelException.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

namespace gm3 {

//##ModelId=3BA8A1C802B4
CProgressCallBack::CProgressCallBack()
: m_lTotalRecords(0), m_lRecordCount(0)
{

}

//##ModelId=3BA8A1C802C7
void CProgressCallBack::OnCancel()
{
	// Throw cancel exeption ....
	throw new CCancelException;

}

//##ModelId=3BA8A1C802B5
void CProgressCallBack::TotalRecords(const long lTotalRecords)
{
	ASSERT(lTotalRecords > 0);
	m_lTotalRecords = lTotalRecords;
}

//##ModelId=3BA8A1C802B7
long CProgressCallBack::TotalRecords() const 
{
	return m_lTotalRecords;
}

//##ModelId=3BA8A1C802C4
void CProgressCallBack::OnNewTable(const CString & strTableName)
{
	// Overload for derived class
}

//##ModelId=3BA8A1C802B9
void CProgressCallBack::OnNewRecord()
{
	m_lRecordCount++;
}

int CProgressCallBack::CurrentRecord() const
{
	return m_lRecordCount;
}
}


