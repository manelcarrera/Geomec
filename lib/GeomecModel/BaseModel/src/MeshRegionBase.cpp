// MeshRegionBase.cpp: implementation of the CMeshRegionBase class.
//
//////////////////////////////////////////////////////////////////////

#include "ValueTypes.h"
#include "MeshRegionBase.h"
#include "GeomecStringTable.h"
#include "resourceIDS.h"

#ifdef _DEBUG
#ifdef _MSC_VER
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#endif  // _MSC_VER
//#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMeshRegionBase::CMeshRegionBase(CFemAppModel& model)
: CColorNode(model)
{
}

CMeshRegionBase::CMeshRegionBase(const QString &strName, CFemAppModel& model)
: CColorNode(strName, model, qRgb(0, 255, 255))
{
}

CMeshRegionBase::CMeshRegionBase(const unsigned int uName, CFemAppModel& model)
: CColorNode(uName, model, qRgb(0, 255 , 255))
{
}

CMeshRegionBase::CMeshRegionBase(const CMeshRegionBase &rhs)
: CColorNode(rhs)
{
}

CMeshRegionBase& CMeshRegionBase::operator=(const CMeshRegionBase &rhs)
{
	CColorNode::operator=(rhs);
	return *this;
}

bool CMeshRegionBase::operator==(const CMeshRegionBase &rhs) const
{
	return CColorNode::operator==(rhs);
}

unsigned int CMeshRegionBase::TypeId() const
{
	return IDT_TREE_MESH_REGIONS;
}

QString CMeshRegionBase::TypeName() const
{
	return getStringTableEntry(IDS_TREE_MESH_REGIONS);
}