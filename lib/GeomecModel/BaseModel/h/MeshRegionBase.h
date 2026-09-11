// MeshRegionBase.h: interface for the CMeshRegionBase class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MESHREGIONBASE_H__BF0F7139_357E_46D2_9423_5CE8FB0ED6E2__INCLUDED_)
#define AFX_MESHREGIONBASE_H__BF0F7139_357E_46D2_9423_5CE8FB0ED6E2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "colornode.h"
#include "GeomecModelVisitor.h"

class CMeshRegionBase : public CColorNode 
{
public:
	CMeshRegionBase(CFemAppModel& model);
	CMeshRegionBase(const QString &strName, CFemAppModel& model);
	CMeshRegionBase(const unsigned int uName, CFemAppModel& model);
	CMeshRegionBase(const CMeshRegionBase &rhs);
	CMeshRegionBase& operator=(const CMeshRegionBase &rhs);
	bool operator==(const CMeshRegionBase &rhs) const;
	virtual unsigned int TypeId() const;
	virtual QString TypeName() const;

  ACCEPT_GEOMECMODELVISITORS(VisitMeshRegionBase);
};


#endif // !defined(AFX_MESHREGIONBASE_H__BF0F7139_357E_46D2_9423_5CE8FB0ED6E2__INCLUDED_)
