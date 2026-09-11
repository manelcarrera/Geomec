#ifndef _ModelData_h_
#define _ModelData_h_

class CModelBase;

#include "AnalysisLogger.h"

class CModelData {
public:
  CModelData(const QString &fileName);

  CModelBase *operator()();
  const CModelBase *operator()() const;

  const CAnalysisLogger &getAnalysisLogger() const;
  const QString &fileName() const;

private:
  CModelData(const CModelData &rhs);
  CModelData &operator=(const CModelData &rhs);

  CAnalysisLogger m_analysisLogger;
  CModelBase *m_modelBase;
  const QString m_fileName;
};

#endif // _ModelData_h_
