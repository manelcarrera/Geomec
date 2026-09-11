// StatusValue.h: interface for the CStatusValue class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_STATUSVALUE_H__7114806B_53D7_49EE_A842_22FF31975FD1__INCLUDED_)
#define AFX_STATUSVALUE_H__7114806B_53D7_49EE_A842_22FF31975FD1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "IValue.h"

class CDsbStatusValue : public geo::IValue {
public:
  // Status
  enum eDsbStatus {
    NOT_CALCULATED = -1,
    UNKNOWN_ERROR = 0,
    OPTIMUM_MUDWEIGHT_FOUND = 1,
    WELL_ALWAYS_STABLE = 2,
    PREMATURE_ANALYSIS_FAILURE = 3,
    WELL_ALWAYS_UNSTABLE = 4,
    UNABLE_DETERMINE_STABLE_MUDWEIGHT = 5
  };
  // Construction
  CDsbStatusValue(); // Constructs NOT_CALCULATED status value

  bool IsCalculated() const;           // Valid if other than NOT_CALCULATED
  virtual bool Valid() const;          // Valid if other than NOT_CALCULATED
  virtual const double &Value() const; // Value assert when !Valid()
  virtual std::string Representation() const;

  eDsbStatus Status() const; // Current status
  void Value(const double &value, eDsbStatus status);
  void Invalidate(); // Reset status to NOT_CALCULATED
  QString StatusText() const;

private:
  eDsbStatus m_status;
  double m_dValue;
  virtual void Value(const double & /*value*/) { assert(false); } // We don't use him
};

#endif // !defined(AFX_STATUSVALUE_H__7114806B_53D7_49EE_A842_22FF31975FD1__INCLUDED_)
