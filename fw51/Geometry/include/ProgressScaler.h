 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */
// IProgressBase.h: interface for the IProgressBase class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(PROGRESSSCALER_H_INCLUDED_)
#define PROGRESSSCALER_H_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "IProgressBase.h"

#include "GeometryExports.h"

/*!
  \class CProgressScaler
  \brief Scales the number of progress steps of a task to a fixed number of steps

  The parent IProgress indicator receives a fixed number of steps scaled to
  the progress of the CProgressScaler task.  

  \sa IProgressBase
*/

class GEOMETRY_EXPORT  CProgressScaler : public IProgressBase
{
	IProgressBase& m_parent;
	int m_nTotalParentSteps;
	int m_nParentSteps;
	int m_nTotalSteps;
	int m_nSteps;
public:
	CProgressScaler(IProgressBase& parent, int nParentSteps);
	virtual ~CProgressScaler();

	// set the total number of steps
	virtual void AddSteps(int nSteps);
	// increment
	virtual void Step(int nSteps = 1);

	// set the title of the progress indicator
	virtual void StatusMessage(const QString& sMessage);
};

#endif // !defined(PROGRESSSCALER_H_INCLUDED_)
