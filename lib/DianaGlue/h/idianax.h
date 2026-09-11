#pragma once

#include <QString>
#include <QDialog>

class IDianaX
{

// Attributes
public:

  /*virtual void SetShowDialog(bool);
  virtual void SetSuccessMessageString(QString);*/
  virtual void SetNumberOfMessages(long)=0;
  /*virtual void SetDisplayStopMessage(bool);
  virtual void SetShowDefaultMessages(bool);
  virtual void SetDialogCaption(QString);
  virtual void SetAutoCloseDialog(bool);
  virtual void SetAlwaysAutoCloseDialog(bool);*/
  virtual void SetProgressIsLifesign(bool)=0;
  /*virtual void SetNumberOfAppSpcMessages(long);

  virtual int GetShowDialog();
  virtual int GetDisplayStopMessage();
  virtual int GetShowDefaultMessages();
  virtual int GetAutoCloseDialog();
  virtual int GetAlwaysAutoCloseDialog();
  virtual int GetProgressIsLifesign();

  virtual QString GetSuccessMessageString();
  virtual QString GetDialogCaption();

  virtual long GetNumberOfMessages();
  virtual long GetNumberOfAppSpcMessages();*/
  

// Operations
public:
  /*virtual void AboutBox();*/

  virtual bool SetEnvironmentVar(QString VarName, QString Value)=0;
  virtual bool GetCalculationResult()=0;
  //virtual int SetUserMagic(QString Leader, QString Trailer);

  virtual short ExecuteDiana(QString ExeName, QString WorkingDir, QString ComFile, QString DatFile, QString FilosFile, QString BaseName)=0;
};

