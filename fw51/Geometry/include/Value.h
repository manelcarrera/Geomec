 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */
// Value.h: interface for the CValue class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VALUE_H__A3A25D88_4996_44AD_96B7_5CB99FE06530__INCLUDED_)
#define AFX_VALUE_H__A3A25D88_4996_44AD_96B7_5CB99FE06530__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "IValue.h"

#include "GeometryExports.h"

namespace geo {

class GEOMETRY_EXPORT  CValue : public IValue
{
#ifdef _DEBUG
	bool m_bValid;
#endif
	double m_dValue;
public:
	CValue();
	CValue(const double &value);
	CValue(const IValue &value);

	CValue& operator=(const IValue &value);
	CValue &operator=(const double &value);

	virtual bool Valid() const;
  virtual void Invalidate();
	virtual const double &Value() const;
	virtual void Value(const double &value);

	virtual std::string Representation() const;
};

}

#endif // !defined(AFX_VALUE_H__A3A25D88_4996_44AD_96B7_5CB99FE06530__INCLUDED_)
