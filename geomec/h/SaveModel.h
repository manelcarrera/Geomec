#ifndef _SaveModel_h_
#define _SaveModel_h_

#include "ISaveModel.h"

class CSaveModel : public ISaveModel {
public:
  CSaveModel(ISaveModel *strategy);
  virtual ~CSaveModel();

  virtual bool saveModel(const QString &fileName, bool bSwitch);

private:
  CSaveModel(const CSaveModel &rhs);
  CSaveModel &operator=(const CSaveModel &rhs);

  ISaveModel *m_strategy;
};

#endif // _SaveModel_h_
