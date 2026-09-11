#include "ProgressFactory.h"
#include "ProgressWrapper.h"
#include "ProgressWrapper_Evt.h"
#include "Progress_.h" //eProgress
#include "ProgressDlg_Qt.h"
#include "Global.h"

//
// FIXME: mcr 2020-08-02
//
//#include "WaitProgress.h"

IProgressBase* CProgressFactory::create(
	eProgress type_,
	const QString& title, 
	bool cancel,
	int jobs)
{
	if(_g->dsa())
	{
		return new CProgressWrapper_Evt(type_,title,cancel,jobs);
	}
	else
	{
		//
		// FIXME: wait case not implemented so don't make this happen ( CProgressFactory_GUI solves this)
		//
		// why don't return create_imp? (in this case wrapper don't add any funtionality...) 
		//		-> because it has some functionality as filtering calls in silent progress behavior for example
		//
		return new CProgressWrapper(type_,title,cancel,jobs);
	}
}

IProgressBase* CProgressFactory::create_imp(
	eProgress type_,
	const QString& title,
	bool cancel,
	int jobs)
{
	return new CProgressDlg_Qt(type_, title, cancel, jobs);
}
