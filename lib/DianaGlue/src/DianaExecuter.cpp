#include "DianaExecuter.h"

#include "DianaStartUp.h"
#include "DianaExecuterSignalHandler.h"

#include "IDCDianaRunner.h" // DianaCore lib
// diana libs
#include "lbcx.h"

//qt
#include <QFile>
#include <QTextStream>


//
// TODO: comment when this class is used
//
// things needs to be removed as they are no longer used
//
CDianaExecuter::CDianaExecuter()
	: m_bShowDefaultMessages(true)
{
	m_handler =  new CDianaExecuterSignalHandler();

	m_dsu = CDianaStartUp::instance();
}

CDianaExecuter::~CDianaExecuter()
{
	delete m_handler;
}

void CDianaExecuter::SetDisplayStopMessage(bool){}

void CDianaExecuter::SetShowDefaultMessages(bool bShow){ m_bShowDefaultMessages = bShow; }

void CDianaExecuter::SetAutoCloseDialog(bool){}

void CDianaExecuter::SetProgressIsLifesign(bool){}


void CDianaExecuter::SetRunner(dia::IDianaRunner* /*runner*/){}




short CDianaExecuter::ExecuteDiana(){		return m_dsu->RunDiana(); }
void CDianaExecuter::ExecuteDianaCleanup(){ m_dsu->RunDianaCleanup(); }

//
// TODO: special case implemented only for Calib case
//
// does this need to be used in other cases: all mc42, in51?
//
bool CDianaExecuter::GetCalculationResult()
{ 
	//
	// implementation before
	//
	//return m_handler->GetCalculationResult(); 
	//

	QString FFDIR = m_dsu->GetDianaEnv( CDianaStartUp::FFDIR );
	QString FF = m_dsu->GetDianaEnv( CDianaStartUp::FF );

	//
	// a) special case: Calib
	//
	QString FF_CALIB = "Calib.ff";
	if( FF == FF_CALIB )
	{
		auto success = [](const QString& filename, const QString& target)
		{
			// modify file in memory
			QFile f(filename);
			f.open(QFile::ReadOnly | QFile::Text);
			QTextStream in(&f);
			QString s = in.readAll();
			return s.contains(target);
		};

		QString BASE_CALIB = FF_CALIB.replace("ff","out");
		QString last_char = FFDIR.right(1);
		if( last_char == "/" || last_char == "\\")
			FFDIR.chop(1);

		QString BASE_FULLPATH = ( QStringList() << FFDIR << BASE_CALIB ).join("/");
		QString SUCCESS_FLAG = "/DIANA/DC/END";
		return success(BASE_FULLPATH,SUCCESS_FLAG);
	}
	//
	// b) regular case
	//
	else
	{
		bool b_HaveResults;
		bool b_CalculationResult;

		dia::IDianaRunner::GetResultInfo(
			FFDIR.toStdString().c_str(), 
			FF.toStdString().c_str(), 
			b_HaveResults, 
			b_CalculationResult);

		return b_HaveResults && b_CalculationResult;
	}
}