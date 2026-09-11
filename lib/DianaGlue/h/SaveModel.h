#pragma once

#include "ISaveModel.h"

class CSaveModel : public ISaveModel {
public:
  CSaveModel() {};
  virtual ~CSaveModel() {}; // =0

  virtual bool saveModel(const QString &fileName, bool bSwitch) { return true; }; // =0

private:
  CSaveModel(const CSaveModel &rhs) {};
  CSaveModel &operator=(const CSaveModel &rhs) {};
};