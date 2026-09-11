#pragma once

class IProgressBase; 

#include "Progress_.h" //eProgress

#include <QString>

class IProgressFactory
{
public:

  virtual IProgressBase* create(
    eProgress type_,
    const QString& title="", 
    bool cancel=true,
    int jobs=1)=0;

  virtual IProgressBase *create_imp(
  eProgress type_,
  const QString& title = "",
  bool cancel = true,
  int jobs = 1) = 0;
};

//
// FIXME: this will crash so by now better don't do it
// 
// to continue IProgressBase needs to be moved from FW51-Geometry lib to QUtil
// but IProgressBase includes dimple.h (Daiana)
// this dependency needs to eb broken but there are 65x files using this diana header
//
class EmptyProgressFactory : public IProgressFactory
{
public:

  virtual IProgressBase* create(
    eProgress type_,
    const QString& title, 
    bool cancel,
    int jobs)
  { 
    return nullptr; // FIXME !!!
  };

  virtual IProgressBase* create_imp(
  eProgress type_,
  const QString& title,
  bool cancel,
  int jobs)
  {
  return nullptr; // FIXME !!!
  };
};