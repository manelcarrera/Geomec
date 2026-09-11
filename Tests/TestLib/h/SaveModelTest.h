#pragma once

class CModelBase;

#include <QString>

#include "ISaveModel.h"

class CSaveModelTest : public ISaveModel
{
public:
  CSaveModelTest(CModelBase& model, const QString& sAppVersion);
  virtual ~CSaveModelTest();

  virtual bool saveModel(const QString& fileName, bool bSwitch);

private:
  //CSaveModelConsole(const CSaveModelConsole& rhs);
  //CSaveModelConsole& operator = (const CSaveModelConsole& rhs);

  CModelBase& m_model;
  QString m_sAppVersion;
};