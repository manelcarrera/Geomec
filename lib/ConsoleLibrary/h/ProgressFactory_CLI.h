#pragma once

class IProgressBase;

#include <QString>

#include "Progress_.h"

#include "IProgressFactory.h"

class CProgressFactory_CLI : public IProgressFactory
{
public:
  IProgressBase* create(
    eProgress type_,
    const QString& title="", 
    bool cancel = true,
    int jobs=1);

  IProgressBase* create_imp(
  eProgress type_,
  const QString& title = "",
  bool cancel = true,
  int jobs = 1);
};