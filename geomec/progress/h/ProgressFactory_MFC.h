#pragma once

#include "Progress_.h" //eProgress
#include "ProgressFactory.h" 

//
// in this case GUI means MFC
//
class CProgressFactory_MFC : public CProgressFactory
{
public:
	virtual IProgressBase* create(
		eProgress type_,
		const QString& title, 
		bool cancel,
		int jobs);

	virtual IProgressBase* create_imp(
		eProgress type_,
		const QString& title,
		bool cancel,
		int jobs);
};

