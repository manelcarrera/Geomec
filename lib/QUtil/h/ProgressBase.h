#pragma once

#include "IProgressBase.h"

class CProgressBase : public IProgressBase
{
  public:
    CProgressBase();
    virtual ~CProgressBase();

  protected:
    static QString getDateTimeStamp();

  private:
    CProgressBase(const CProgressBase& rhs);
    CProgressBase& operator = (const CProgressBase& rhs);
};
