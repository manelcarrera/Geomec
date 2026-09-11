#pragma once

#include <QString>
#include <QDialog>

#include "idianax.h"

class CDianaXQt : public IDianaX, public QDialog
{

// Attributes
public:
	virtual void SetShowDialog(bool);
	virtual void SetSuccessMessageString(QString);
	virtual void SetNumberOfMessages(long);
	virtual void SetDisplayStopMessage(bool);
	virtual void SetShowDefaultMessages(bool);
	virtual void SetDialogCaption(QString);
	virtual void SetAutoCloseDialog(bool);
	virtual void SetAlwaysAutoCloseDialog(bool);
	virtual void SetProgressIsLifesign(bool);
	virtual void SetNumberOfAppSpcMessages(long);

	virtual bool GetShowDialog();
	virtual bool GetDisplayStopMessage();
	virtual bool GetShowDefaultMessages();
	virtual bool GetAutoCloseDialog();
	virtual bool GetAlwaysAutoCloseDialog();
	virtual bool GetProgressIsLifesign();

	virtual QString GetSuccessMessageString();
	virtual QString GetDialogCaption();

	virtual long GetNumberOfMessages();
	virtual long GetNumberOfAppSpcMessages();

// Operations
public:
	virtual void AboutBox();

	virtual bool SetEnvironmentVar(QString VarName, QString Value);
	virtual bool GetCalculationResult();
	virtual bool SetUserMagic(QString Leader, QString Trailer);

	virtual short ExecuteDiana();
};
