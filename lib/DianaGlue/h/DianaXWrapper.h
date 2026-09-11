#pragma once

#include "IDianaXWrapper.h"
//#include "ExecuteDianaDialog.h"

class IExecuteDianaDialog;

#include <QString>

//template< T >
class CDianaXWrapper : public IDianaXWrapper
{
	//CExecuteDianaDialog m_executeDianaDialog;
	IExecuteDianaDialog* m_exe_diana_dlg;

public:
	CDianaXWrapper( IExecuteDianaDialog* exe_diana_dlg );
	virtual ~CDianaXWrapper();

	virtual void SetDisplayStopMessage(bool);
	virtual void SetShowDefaultMessages(bool);
	virtual void SetAutoCloseDialog(bool);

	virtual void SetProgressIsLifesign(bool);

	virtual void SetRunner(dia::IDianaRunner *runner);

	virtual short ExecuteDiana();
  virtual void ExecuteDianaCleanup();

	virtual bool GetCalculationResult();
};