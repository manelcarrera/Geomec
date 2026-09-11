#pragma once

#include <QString>

#include "IDCAnalysisStatus.h"
#include "IDCDianaRunner.h"

#include "lbcx.h" //vDiSetenv

#include "IExeDianaDlg.h"


static const QString DEF_SUCCEED_MSG = "/DIANA/DC/END";

class IExecuteDianaDialog : public IExeDianaDlg
{
protected:
	dia::IDianaRunner* m_runner;

	bool m_bCalculationSucceeded;

	QString m_succeed_msg;

public:
	IExecuteDianaDialog()
    : m_runner(nullptr)
	{
		m_succeed_msg = DEF_SUCCEED_MSG;
	};

	virtual ~IExecuteDianaDialog(){};

	void SetRunner(dia::IDianaRunner *runner){ m_runner = runner; };


	virtual short ExecuteDiana() = 0;
  virtual void ExecuteDianaCleanup() = 0;

	bool GetCalculationResult(){ return m_bCalculationSucceeded; }

	QString GetSuccessMessageString(){ return  m_succeed_msg; }
	void SetSuccessMessageString( QString val ) 
	{
		if( !val.isEmpty() )
			m_succeed_msg = val;
	}

	virtual void OnMessage(QString str)
	{
		if( str.indexOf( m_succeed_msg ) != -1 )
			m_bCalculationSucceeded = true;
	};

	virtual void OnAppSpcMessage(QString str) = 0;
	virtual void OnError(QString str) = 0;			
	virtual void OnWarning(QString str) = 0;			
	virtual void OnStopExecuting(bool &bStop) = 0;
	void OnFinished(){ Finish(); };

	virtual void SetAutoCloseDialog(bool bNewValue) = 0;
	virtual void SetProgressIsLifesign(bool bNewValue) = 0;
	virtual void SetDisplayStopMessage(bool bNewValue) = 0;
	virtual void SetShowDefaultMessages(bool bNewValue) = 0;

protected:


	void ProcessStatus(QString Message)
	{
		if( m_runner )
		{
			dia::IAnalysisStatus *pStatus = m_runner->AnalysisStatus();
			if( pStatus )
				pStatus->ParseMessage(Message.trimmed());
		}
	}

	virtual void Finish()
	{
    if (!m_bCalculationSucceeded && m_runner)
    {
      bool bHaveResults = false;
      m_runner->GetResultInfo(bHaveResults, m_bCalculationSucceeded);
    }
	};

	virtual int CallBack() = 0;
};
