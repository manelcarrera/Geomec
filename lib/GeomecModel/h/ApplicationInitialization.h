#ifndef _ApplicationInitialization_h_
#define _ApplicationInitialization_h_

#include <QString>
// #include "../../../geomec3/version.h"
#include "version.h"

const QString APPLICATION_NAME = "GEOMEC " xstr(GM_MAJOR) "." xstr(GM_MINOR);
const QString COMPANY_NAME = "TNO";
const QString FILE_NAME = "geomec.ini";

class CApplicationInitialization {
public:
  CApplicationInitialization(const QString &applicationName = APPLICATION_NAME,
                             const QString &companyName = COMPANY_NAME, const QString &fileName = FILE_NAME);
  ~CApplicationInitialization();

  const QString &applicationName() const;

private:
  CApplicationInitialization(const CApplicationInitialization &rhs);
  CApplicationInitialization &operator=(const CApplicationInitialization &rhs);

  const QString &m_applicationName;
};

#endif // _ApplicationInitialization_h_
