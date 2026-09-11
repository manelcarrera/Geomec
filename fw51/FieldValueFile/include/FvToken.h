 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */
// Token.h: interface for the CToken class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TOKEN_H__3E14E7D7_C112_40AF_8A0F_6F5A794F3607__INCLUDED_)
#define AFX_TOKEN_H__3E14E7D7_C112_40AF_8A0F_6F5A794F3607__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <qstring.h>

#include "FieldValueFileExports.h"

// CToken represents the base class of a keyword. The keyword is stored in m_sToken. When the
// keyword is detected in the file the Read() function is called for reading a specialised
// data section in the file and returns the TokenRet status
class input_stream;
class FIELDVALUEFILE_EXPORT CToken 
{
  QString m_sToken;
public:
  CToken(const  QString &sToken);
  virtual ~CToken() = 0;

  /*!
     The token function executed when a keyword is read
     Returns true for continuation and false when the job is finished correctly
  */
  virtual bool Read(input_stream& stream, const QString &sToken) = 0;

  const  QString& Token() const;
  bool operator<(const CToken &rhs) const;
};

/*!
  \brief This template class is instantiated for the class T the token function must work on.
*/
template<class T>
class CTokenTemplate : public CToken
{
public:
  // The token function of the type TokenRet T::[FUNCTION_NAME](const QString& sToken)
  typedef bool (T::*pfToken)(input_stream& stream, const QString &sToken);
private:
  // Pointer to the class T we execute function for ...
  T*      m_pInstance;
  pfToken    m_pTokenFunction;
public:

  // Constructor ....
  CTokenTemplate(T& instance, const QString &sToken, pfToken token)
  : CToken(sToken), m_pInstance(&instance), m_pTokenFunction(token)
  {
  };

  // Execute token function in the override of the read function
  virtual bool Read(input_stream& stream, const QString &sToken)
  {
     return (m_pInstance->*m_pTokenFunction)(stream, sToken);
  }
};

#endif // !defined(AFX_TOKEN_H__3E14E7D7_C112_40AF_8A0F_6F5A794F3607__INCLUDED_)
