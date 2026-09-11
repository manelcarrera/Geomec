#if !defined(GVTINPUTWRITER_H_)
#define GVTINPUTWRITER_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <stdio.h>

#include <qstring.h>

class CGVTSettings;

class CGVTInputWriter {
  const CGVTSettings &m_GVTSettings;
  FILE *m_fp;
  QString m_strError;

public:
  CGVTInputWriter(const CGVTSettings &settings);
  ~CGVTInputWriter(void);

  bool WriteInput(const QString &sPath);
  const QString &Error() const { return m_strError; }

private:
  bool WriteOutputSection();
  bool WriteMessageFileSection();
  bool WriteVelModelSection();
  bool WriteCalcTimesSection();
  bool WritePointSetSection();
};

#endif