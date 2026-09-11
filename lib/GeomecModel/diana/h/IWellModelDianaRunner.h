#ifndef _IWELLMODELDIANARUNNER_H_
#define _IWELLMODELDIANARUNNER_H_

#include "GeomecCompactionDianaRunner.h"

class IWellModel;

class IWellModelDianaRunner : public CGeomecCompactionDianaRunner {
public:
  IWellModelDianaRunner(IWellModel &model, CDianaRunController &controller);
  virtual ~IWellModelDianaRunner();

  virtual bool CreateTopLoad(const CHorizonBase &top, dia::CLoadCase &lcase);
  virtual void CreateWeightLoad(dia::CLoadCase &lcase, const double &gravity);

  virtual bool TranslateElementsToXYOrigin() const { return true; }

protected:
  bool ParentLinearResults() const;
  virtual int LoadsProgressSize() const;
  virtual bool CheckConvergenceMinimumVolume() const { return false; }
};

#endif // _IWELLMODELDIANARUNNER_H_
