// MeshProgress.cpp: implementation of the CTetraMesh class.
//
//////////////////////////////////////////////////////////////////////

#include "MeshProgress.h"
#include "IProgressBase.h"

#include <qstring.h>

#ifdef _DEBUG
#ifdef _MSC_VER
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#endif  // _MSC_VER
//#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Implementation of mesh progress
//////////////////////////////////////////////////////////////////////
CMeshProgress::CMeshProgress(IProgressBase& dlg)
: m_nTotalSteps(-1), m_nCurrentStep(0), m_dlg(dlg)
{
}

void CMeshProgress::StartMesh(int nJobs)
{
	m_nTotalSteps = nJobs;
	m_dlg.AddSteps(m_nTotalSteps);
}

void CMeshProgress::NewJob(std::string sJobName)
{
	assert(m_nTotalSteps > 0);
  m_dlg.StatusMessage(sJobName.c_str());
	m_dlg.Step();
	m_nCurrentStep++;
}

void CMeshProgress::StopMesh()
{
}
