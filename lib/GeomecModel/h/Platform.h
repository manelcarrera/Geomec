#ifndef _PLATFORM_H_
#define _PLATFORM_H_

#include "IPlatform.h"

class CPlatform : public IPlatform
{
public:
  static void init();

  virtual long processMemorySize() const;
  virtual void trace(const QString& message) const;

private:
  CPlatform();
  CPlatform(const CPlatform& rhs);
  CPlatform& operator = (const CPlatform& rhs);
};

#endif // _PLATFORM_H_
