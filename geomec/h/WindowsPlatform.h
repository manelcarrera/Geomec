#ifndef _WindowsPlatform_h_
#define _WindowsPlatform_h_

#include "IPlatform.h"

class CWindowsPlatform : public IPlatform
{
public:
  CWindowsPlatform();

  static void init();

  virtual long processMemorySize() const;
  virtual void trace(const QString& message) const;

private:
  CWindowsPlatform(const CWindowsPlatform& rhs);
  CWindowsPlatform& operator = (const CWindowsPlatform& rhs);
};

#endif  // _WindowsPlatform_h_
