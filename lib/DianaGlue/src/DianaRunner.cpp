
//qt
#include <qstring.h>
#include <qfile.h>
#include <qtextstream.h>
//own
#include "DianaRunner.h"
#include "IDianaXWrapper.h"
#include "DianaStartUp.h"
#include "GlobalMessage.h"
#include "Printer.h"
#include "Environment.h"

namespace
{
	Printer* printer = Printer::instance(Printer::RunR);
}

CDianaRunner::CDianaRunner(const geo::IMesh &mesh)
:	dia::IDianaRunner(mesh)
{
}

CDianaRunner::~CDianaRunner()
{
}

//
//	dsa: results still not available
//
bool CDianaRunner::on_run_analysis_dsa(bool bSilent)
{
	CDianaStartUp* dsu = CDianaStartUp::instance();

	dsu->SetDianaEnv( CDianaStartUp::FF, FF().c_str() );
	dsu->SetDianaEnv( CDianaStartUp::FFDIR, FFDIR().c_str() );

	dsu->SetDianaEnv( 
		CEnvironment::STP0, 
		dsu->GetDianaEnv( CEnvironment::DIAPATH ) + "/binseg/ap/" + DianaExecutable() );

	dsu->RunDiana();
	return true;

	/*if (bSilent)
		short res = dsu->RunDiana();
	else // non-silent: interactive
	{
		PreExecution();
		short res = dsu->RunDiana();
		dsu->RunDianaCleanup();			// clear vars and FF and FFDIR
	}
	return true;*/
}

bool CDianaRunner::OnRunAnalysis(IDianaXWrapper* dianaXWrapper, bool bSilent)
{
	printer->info("on run analysis (%s)", m_params.dsa ? "dsa":"non-dsa");

	return m_params.dsa ? 
		on_run_analysis_dsa(bSilent) : 
		OnRunAnalysis_(dianaXWrapper,bSilent);
}

//
// non-dsa
//
bool CDianaRunner::OnRunAnalysis_(IDianaXWrapper* dianaXWrapper, bool bSilent)
{
	printer->info("on run analysis");

	//
	//
	//
	if( m_params.dsa )
		return on_run_analysis_dsa(bSilent);
	//
	//
	//

	CDianaStartUp* dsu = CDianaStartUp::instance();

	dsu->SetDianaEnv( CDianaStartUp::FF, FF().c_str() );
	dsu->SetDianaEnv( CDianaStartUp::FFDIR, FFDIR().c_str() );

	//
	// mcr 2020-02-17
	//
	// CEnvironment::STP0:
	//
	// by now it needs to be done here
	// even if the default one is set on launching the app
	// this is because it can take two values:
	// -CEnvironment::InputExeName -> in51.exe
	// -CEnvironment::DianaExeName -> gm42.exe
	// and it is the controller the one who knows it
	//
	// For further developements this can be done at RunAnalysis::run
	// depending on the param 'bool bWriteInputFiles'
	// so no need to pass the param along all the classes chain
	//
	dsu->SetDianaEnv( 
		CEnvironment::STP0, 
		dsu->GetDianaEnv( CEnvironment::DIAPATH ) + "/binseg/ap/" + DianaExecutable() );


	dianaXWrapper->SetShowDefaultMessages(DefaultMessages());
	dianaXWrapper->SetDisplayStopMessage(DisplayStopMessage());
	dianaXWrapper->SetAutoCloseDialog(AutoCloseDialog());

	dianaXWrapper->SetRunner(this);

	bool bRetry = true;
	bool bCalcResult = false;
	bool bHaveResults = false;


	if (bSilent)
	{
		dianaXWrapper->SetShowDefaultMessages(false);
		short res = dianaXWrapper->ExecuteDiana(); // 0 is Ok
		GetResultInfo(bHaveResults, bCalcResult);
	}
	//
	// non-silent: interactive
	//
	else
	{
		int auto_retry = LicenseRetry();
		bool retry_is_auto = auto_retry > 0;

		while (bRetry)
		{
			//
			// mcr 2020-04-07
			// 
			// related to retries when license check fails
			// no need to clean up run diana vars at the end if each retry vars are set
			// since they are available as class members 
			//
			// dsu->SetDianaEnv( CDianaStartUp::FF, FilosFileName().c_str() );
			// dsu->SetDianaEnv( CDianaStartUp::FFDIR, WorkingDir().c_str() );
			//

		  bRetry = false;

		  PreExecution();

		  short res = dianaXWrapper->ExecuteDiana();


			if (res)
				ErrorRunning();

			GetResultInfo(bHaveResults, bCalcResult);

			printer->info("results : %s", bCalcResult ? "yes" : "no");

			if (!bCalcResult)
			{
				QString licenseError = LicenseError();
				//
				// license error
				//
				if (!licenseError.isEmpty())
				{
					printer->error("license error : %s", licenseError.toStdString().c_str());

					if (retry_is_auto)
					{
						bRetry = auto_retry-- > 0;

						if (bRetry)
							licenseError += "\nRetrying calculation...";
						_m()->warn(licenseError);
					}
					else
					{
						licenseError += "\nDo you want to try again?";
						bRetry = _m()->msg(licenseError, MB_YESNO) == IDYES;
					}
				}
				//
				// not license error
				//
				else
				{
					if (bHaveResults)
					{
						if (_m()->msg("Analysis failed but there are results (partially) available; do you want to try to load them?", MB_YESNO) == IDYES)
						{
							if (!LoadPartialResults())
							{
								_m()->msg(QString("Loading partial results from '%1' failed; please copy the filos file and try again manually.").arg(FF().c_str()));
							}
						}
					}
				}
			}

      if (!bRetry)
        dianaXWrapper->ExecuteDianaCleanup();

			PostExecution(bCalcResult);
		} //while (bRetry)
	} //else

	if( !bCalcResult )
		printer->error("error : no results");

	return (bCalcResult != false);
}


QString CDianaRunner::LicenseError()
{
  QStringList licenseError;

  QFile file(QString(FFDIR().c_str()) + "/DIANA.OUT");
  if (file.open(QIODevice::ReadOnly | QIODevice::Text))
  {
    int nState = 0;

    QTextStream stream(&file);
    QString line = stream.readLine();
    while (!line.isNull())
    {
      switch (nState)
      {
      case 0:
        if (line.contains("SEVERITY  : ABORT"))
          nState = 1;
        break;
      case 1:
        if (line.contains("ERROR CODE: /DIANA/LB/AU"))
          nState = 2;
        break;
      case 2:
        if (line.contains("DIANA-JOB ABORTED"))
          nState = 3;
        else
        {
          if (line.contains("ERRORMSG.A: "))
            line = line.right(line.length() - 13);
          licenseError.append(line);
        }
      }
      line = stream.readLine();
    }
  }

  return licenseError.join("\n");
}


bool CDianaRunner::LoadPartialResults()
{
  return false;
}