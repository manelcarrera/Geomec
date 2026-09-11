#pragma once

// qt
#include <QString>
// own
#include "IProgressFactory.h"
#include "Progress_.h" //eProgress

class IProgressBase;

//
// this is both GUI (Qt, not MFC) and events driven depending on dsa/non-dsa mode
//
class CProgressFactory : public IProgressFactory {
public:
  virtual IProgressBase *create(eProgress type_, const QString &title = "", bool cancel = true, int jobs = 1);

  virtual IProgressBase *create_imp(eProgress type_, const QString &title = "", bool cancel = true, int jobs = 1);
};