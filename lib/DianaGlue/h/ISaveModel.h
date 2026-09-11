#pragma once

class QString;

class ISaveModel {
public:
  ISaveModel();          //{};
  virtual ~ISaveModel(); //{}

  virtual bool saveModel(const QString &fileName, bool bSwitch) = 0;
};
