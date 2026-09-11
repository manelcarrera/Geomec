// WellPathFile.h: interface for the CWellPathFile class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WELLPATHFILE_H__B2043099_2BDB_4F7E_94E0_652A01ED72E0__INCLUDED_)
#define AFX_WELLPATHFILE_H__B2043099_2BDB_4F7E_94E0_652A01ED72E0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CNewWellPathInput;

#include "FvTextFile.h"
#include "SingleQuantity.h"

class CWellPathFile : public CTextFile
{
  enum eState {READ_NONE, READ_HEADER, READ_VALUE, READ_FINISHED, FORMAT_ERROR};
  eState m_State;
  typedef CTokenTemplate<CWellPathFile> TWellPathToken;
  CNewWellPathInput *m_pNewWellPathInput;

  void CheckDelimiter(TInputStream& stream, char ch);

private:
  CQuantity::UNIT m_Unit;

  double m_Easting;
  double m_Northing;
  double m_TVD;

public:
  CWellPathFile(CNewWellPathInput *pWellPathInput,CQuantity::UNIT unit = CQuantity::SI_UNIT);
  virtual ~CWellPathFile();
  
  bool ReadIgnore(TInputStream& stream, const QString& sToken);
  bool ReadWellName(TInputStream& stream, const QString& sToken);
  bool ReadNorthing(TInputStream& stream, const QString& sToken);
  bool ReadEasting(TInputStream& stream, const QString& sToken);
  bool ReadDerrickElev(TInputStream& stream, const QString& sToken);

  virtual void OnParseSucceed(TInputStream& stream, const QString& sToken);	// The parsing of a token was OK
};

#endif // !defined(AFX_WELLPATHFILE_H__B2043099_2BDB_4F7E_94E0_652A01ED72E0__INCLUDED_)
 
