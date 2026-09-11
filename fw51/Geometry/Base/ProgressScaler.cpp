 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */
// ProgressScaler.cpp: implementation of the CProgressScaler class.
//
//////////////////////////////////////////////////////////////////////

#include "dimple.h"
#include "ProgressScaler.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
   
CProgressScaler::CProgressScaler(IProgressBase& parent, int nParentSteps)
: m_parent(parent),
  m_nTotalParentSteps(nParentSteps),
  m_nParentSteps(0),
  m_nTotalSteps(0),
  m_nSteps(0)
{
}

CProgressScaler::~CProgressScaler()
{
}

void CProgressScaler::AddSteps(int nSteps)
{
  m_nTotalSteps += nSteps;
}

void CProgressScaler::Step(int nSteps)
{
  m_nSteps += nSteps;
  int nParentSteps = (m_nParentSteps * m_nSteps) / m_nTotalSteps;
  int nParentStepsToMake = m_nParentSteps - nParentSteps;
  if(nParentStepsToMake > 0)
    m_parent.Step(nParentStepsToMake);
  m_nParentSteps = nParentSteps;
}

void CProgressScaler::StatusMessage(const QString& sMessage)
{
  m_parent.StatusMessage( sMessage );
}
