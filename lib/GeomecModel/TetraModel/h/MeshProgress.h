// MeshProgress.h: interface for the CTetraMesh class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MESHPROGRESS_H__8CD2147B_DC40_40ca_8DD2_92C7466395A6__INCLUDED_)
#define AFX_MESHPROGRESS_H__8CD2147B_DC40_40ca_8DD2_92C7466395A6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "TetMeshBase.h"

//#define MESH_LOADED	350

class IProgressBase;

// Class for progress indication
class CMeshProgress : public geo::CTetMeshProgress
{
	int m_nTotalSteps;
	int m_nCurrentStep;
	IProgressBase& m_dlg;
public:
	CMeshProgress(IProgressBase& dlg);
	virtual void StartMesh(int nJobs);
	virtual void NewJob(std::string sJobName);
	virtual void StopMesh();
};

#endif // !defined(AFX_MESHPROGRESS_H__8CD2147B_DC40_40ca_8DD2_92C7466395A6__INCLUDED_)
