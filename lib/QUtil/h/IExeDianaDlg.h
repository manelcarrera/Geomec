#pragma once

#include "EventsDiana_.h"

#include <vector>
class QString;

//
// class empty of functionality
//
class IExeDianaDlg
{
public:
  //
  // ExecuteDianaDialogQt
  //
  virtual void msg(const std::vector<QString>& val, eDianaMsg type_){};
  virtual void msg(const QString& val, eDianaMsg type_){};

  virtual void show_(bool val=true){};
  virtual void hide_(){};
  virtual void clear(){};


  //
  // ExecuteDianaDialog (MFC)
  //
  virtual void OnMessage(QString str){};
  virtual void OnAppSpcMessage(QString str){};
  virtual void OnError(QString str){};			
  virtual void OnWarning(QString str){};			
  virtual void OnStopExecuting(bool &bStop){};
  virtual void OnFinished(){};

  //
  // common
  //
  virtual ~IExeDianaDlg(){}
};
