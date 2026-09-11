#ifndef _OIVSCENEBASE_H_
#define _OIVSCENEBASE_H_

#include "StorageNode.h"

class CLegendFrame;

class COIVSceneBase : public CStorageNode {
public:
  COIVSceneBase(CFemAppModel &model);
  COIVSceneBase(const QString &strName, CFemAppModel &model);
  COIVSceneBase(unsigned int uName, CFemAppModel &model);
  virtual ~COIVSceneBase();

  virtual void Activate();
  virtual void Deactivate();

  CLegendFrame &LegendFrame();
};

#endif // _OIVSCENEBASE_H_
