// CommandLineInfoEx.cpp: implementation of the CCommandLineInfoEx class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CommandLineInfoEx.h"

#ifdef _DEBUG
#ifdef _MSC_VER#undef THIS_FILE
static char THIS_FILE[]=__FILE__;#endif  // _MSC_VER
#define new DEBUG_NEW
#endif

//////////////////
// Parse a command line parameter/token. Just add it to the table.
// 
void CCommandLineInfoEx::ParseParam(const TCHAR* pszParam, BOOL bFlag, 
                                    BOOL bLast)
{
   if (bFlag) {
      // this is a "flag" (begins with / or -)
      m_options[pszParam] = "TRUE";    // default value is "TRUE"
      m_sLastOption = pszParam;        // save in case other value specified

   } else if (!m_sLastOption.IsEmpty()) {
      // last token was option: set value
      m_options[m_sLastOption] = pszParam;
      m_sLastOption.Empty(); // clear
   }

   // Call base class so MFC can see this param/token.
   CCommandLineInfo::ParseParam(pszParam, bFlag, bLast);
}

BOOL CCommandLineInfoEx::GetOption(LPCTSTR option, CString& val)
{
   return m_options.Lookup(option, val);
}

