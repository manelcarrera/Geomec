#ifndef _DIANA_LICENSE_CHECK_H_
#define _DIANA_LICENSE_CHECK_H_

#include <QString>
#include "AnalysisLogger.h"
#include "tbb/tbb.h"

class CModelBase;


class CDianaLicenseCheck
{
  bool            m_active;

  const QString   m_modelName;
  CModelBase     *m_pModel;
  CAnalysisLogger m_logger;

  tbb::tick_count m_lastCheck;
  bool            m_lastResult;

  CDianaLicenseCheck();
  CDianaLicenseCheck(const CDianaLicenseCheck&);
  CDianaLicenseCheck& operator=(const CDianaLicenseCheck&);
  ~CDianaLicenseCheck();
public:
  static CDianaLicenseCheck *instance();

  void Activate();
  bool FoundLicense();

  CModelBase* model(){ return m_pModel; }
};



#endif