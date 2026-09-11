// ICtrlObjectBase.h: interface for the ICtrlObjectBase class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ICTRLOBJECTBASE_H__00AB66A4_FC6F_11D5_B8F6_88B2EF8ABF79__INCLUDED_)
#define AFX_ICTRLOBJECTBASE_H__00AB66A4_FC6F_11D5_B8F6_88B2EF8ABF79__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <QString>

class ICtrlObjectBase {
protected:
  ICtrlObjectBase();
  virtual ~ICtrlObjectBase();

public:
  // Text
  virtual QString Text() const = 0;
  virtual BOOL CanEditText() const;
  virtual BOOL EditText(const QString &strText);

  // Less
  virtual BOOL operator<(const ICtrlObjectBase &object) const;
};

#endif // !defined(AFX_ICTRLOBJECTBASE_H__00AB66A4_FC6F_11D5_B8F6_88B2EF8ABF79__INCLUDED_)
