// GM3Database.cpp: implementation of the CGM3Database class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GM3Database.h"
#include "GM3TableDef.h"
#include "gm3IncompleteException.h"
/////////////////////////////////////////////////////////////////////
// Implementation of C3DDatabase Base class
//////////////////////////////////////////////////////////////////////
namespace gm { 

C3DDatabase::C3DDatabase()
{
}

void C3DDatabase::OpenDB(const CString &strPathName, const BOOL bComplete)
{
#ifndef _WIN64
  // Set the complete flag
  m_bComplete = bComplete;

  // Open the database
  Open(strPathName, TRUE, TRUE);

  // Open recordset to obtain version info
  CDaoRecordset rs(this);
  CString strQuery = "[";
  strQuery += TB_GLOBAL_SETTINGS;
  strQuery += "]";
  rs.Open(dbOpenTable, strQuery, dbReadOnly);

  if (rs.IsEOF()) 
  {
    // Throw incomplete exception ...
    throw new CIncompleteException();
  }

  rs.MoveFirst();

  // Apperently we have a Version Field and we have now read the information
  m_dVersion = atof(rs.GetFieldValue(FD_VERSION_DB).pcVal);

  rs.Close();
#endif
}

}