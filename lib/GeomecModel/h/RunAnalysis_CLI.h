#pragma once

class CDianaStartUp;
class IDianaXWrapper;

#include "IRunAnalysis.h"
#include "DianaRunController.h"
#include "Wait.h"

class CRunAnalysis_CLI : public IRunAnalysis
{
	CDianaRunController* m_drc;
	IDianaXWrapper* m_dxw;
	bool m_res;
	CDianaRunController::eRunStep m_step;
	CWait m_wait;

	enum eScenarios{ DianaRunController, DianaXWrapper, DianaStartUp};
	eScenarios m_scenario;

	bool m_quit;
	void quit(bool val){m_quit=val;}

private:

	void handle(Cmd cmd); //=0
	//
	std::string error_msg(Cmd cmd);
	void on_diana_msg(Cmd cmd);

	void init();

public:
	//
	// with no params:
	//
	// to run diana directly without controller and runner
	// results in FF are not set to the model (the one in memory)
	// mainly implemented for ModelOperations.StartDiana
	//
	CRunAnalysis_CLI(
		CDianaRunController* drc, 
		CDianaRunController::eRunStep step);
	//
	CRunAnalysis_CLI();
	//
	CRunAnalysis_CLI(IDianaXWrapper* dxw);

	~CRunAnalysis_CLI();

	void wait();

	bool res(){ return m_res; };

	bool quit(){return m_quit;}	
};
