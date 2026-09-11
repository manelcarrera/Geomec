// TetraSuperHorizon.cpp: implementation of the CTetraSuperHorizon class.
//
//////////////////////////////////////////////////////////////////////

#include "TetraSuperHorizon.h"
#include "TetraEntryTypes.h"
#include "TetraModel.h"
#include "BoundaryBase.h"
#include "TetraSubBoundary.h"
#include "FieldFactor.h"
#include "TetraSurface.h"
#include "BranchState.h"
#include "GlobalMessage.h"
#include "FemAppModel.h"
#include "ModelBase.h"
#include "resourceIDI.h"
#include "resourceIDS.h"
#include "TetraSubHorizon.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTetraSuperHorizon::CTetraSuperHorizon(CFemAppModel& model)
: CTetraHorizonBase(model)
{

}

CTetraSuperHorizon::CTetraSuperHorizon(const QString& sName, const double& dDepth, CFemAppModel& model, bool bAttachToEntry)
: CTetraHorizonBase(sName, dDepth, model, bAttachToEntry)
{
	init();

	if(bAttachToEntry)
	{
		assert(Model().GraphEntry(MD_TETRA_SUPER_HORIZON));
		reParent(Model().GraphEntry(MD_TETRA_SUPER_HORIZON));
	}

	// Link to boundary
	CModelBase *pModel = dynamic_cast<CModelBase*>(&Model());
	LinkTo(pModel->Boundary());
}

CTetraSuperHorizon::~CTetraSuperHorizon()
{

}

CTetraSuperHorizon::CTetraSuperHorizon(CSurfaceBase& surface, CFemAppModel& model, bool bSlip, bool bAttachToEntry)
: CTetraHorizonBase(surface, model, bSlip, bAttachToEntry)
{
	init();

	if(bAttachToEntry)
	{
		assert(Model().GraphEntry(MD_TETRA_SUPER_HORIZON));
		LinkTo(*Model().GraphEntry(MD_TETRA_SUPER_HORIZON));
	}

	// Link to boundary
	CModelBase *pModel = dynamic_cast<CModelBase*>(&Model());
	LinkTo(pModel->Boundary());
}

void CTetraSuperHorizon::LoadStream(TSTREAM& stream, CStreamVersion& version, TPROGRESS& progress)
{
	CTetraHorizonBase::LoadStream(stream, version, progress);
	assert(Model().GraphEntry(MD_TETRA_SUPER_HORIZON));
	reParent(Model().GraphEntry(MD_TETRA_SUPER_HORIZON));
	// Link to boundary
	CModelBase *pModel = dynamic_cast<CModelBase*>(&Model());
	LinkTo(pModel->Boundary());
}

bool CTetraSuperHorizon::ConnectItem(const CGraphNode &item)
{
	CSurfaceBase *pSurf = const_cast<CSurfaceBase*> (dynamic_cast<const CSurfaceBase*> (&item));
	if (pSurf)
	{
		if(!IsLinkedTo(item))
		{
			if(SurfaceSize() == 0)
			{
				LinkTo(const_cast<CGraphNode&>(item));
				Modified();
				return true;
			}
			else
			{
				assert(SurfaceSize() == 1);
				QString sMessage;
				sMessage = QString("Surface %1 is dropped on horizon %2. Do you want to merge them?").arg(item.Name()).arg(Name());
				if(_m()->msg(sMessage, MB_ICONQUESTION | MB_YESNO) == IDYES)
					MergeSurface(*pSurf);
				return true;
			}
		}
	}

	return CTetraHorizonBase::ConnectItem(item);
}

bool CTetraSuperHorizon::CanConnectItem(const CGraphNode &item) const
{
	CSurfaceBase *pSurf = const_cast<CSurfaceBase*>(dynamic_cast<const CSurfaceBase*> (&item));

	if(pSurf)
		return !IsLinkedTo(item) && pSurf->Used() == 0 && !(static_cast<const CModelBase&>(Model())).BranchState().IsBranch();

	return CTetraHorizonBase::CanConnectItem(item);
}

bool CTetraSuperHorizon::Less(const CGraphNode &node) const
{
	const CTetraSuperHorizon *pHorizon = dynamic_cast<const CTetraSuperHorizon*>(&node);
	if(pHorizon)
	{
		if(pHorizon->ConstantDepth())
		{
			if(ConstantDepth())
				return Depth().Value() < pHorizon->Depth().Value();
			return Surface(0).Max().Z() < pHorizon->Depth().Value();
		}
		else
		{
			if(ConstantDepth())
				return Depth().Value() < pHorizon->Surface(0).Max().Z();
			return Surface(0).Max().Z() < pHorizon->Surface(0).Max().Z();
		}
	}

	return CTetraHorizonBase::Less(node);
}

bool CTetraSuperHorizon::IsTopHorizon() const
{
	assert(Model().GraphEntry(MD_TETRA_SUPER_HORIZON));
	const CTetraSuperHorizonEntry* pEntry = dynamic_cast<const CTetraSuperHorizonEntry*>(Model().GraphEntry(MD_TETRA_SUPER_HORIZON));
	assert(pEntry);
	CTetraSuperHorizonEntry::TSortedNodeSet stHorizon = pEntry->SortedEntryNodes();
	if(stHorizon.size() == 0)
		return false;

	return *stHorizon.begin() == this;
}

bool CTetraSuperHorizon::CanSlip() const
{
#ifdef TETRA_FAULTS_KEYFILE
	const CKeyFile &keyfile = (dynamic_cast<const CGeomecApp *> (AfxGetApp()))->KeyFile();
	if(!keyfile.Unlocked()) return false;
#endif

	// Get the entry and walk over the horizons
	assert(Model().GraphEntry(MD_TETRA_SUPER_HORIZON));
	const CTetraSuperHorizonEntry* pEntry = dynamic_cast<const CTetraSuperHorizonEntry*>(Model().GraphEntry(MD_TETRA_SUPER_HORIZON));
	assert(pEntry);
	CTetraSuperHorizonEntry::TSortedNodeSet stHorizon = pEntry->SortedEntryNodes();
	if(stHorizon.size() == 0)
		return false;

	if((*stHorizon.begin()) == this)
		return false;

	CTetraSuperHorizonEntry::TSortedNodeSet::iterator it = stHorizon.end();
	it--;
	if((*it) == this)
		return false;

	return true;
}

bool CTetraSuperHorizon::IsDepthValid(const double& depth, const CLengthQuantity::UNIT unit)
{
	if(!C3DHorizon::IsDepthValid(depth, unit))
		return false;

	double si_depth = depth;
	if(unit == CQuantity::FIELD_UNIT)
		si_depth = si_depth / FF_FACTOR_LENGTH;

	const CTetraModel& model = dynamic_cast<const CTetraModel&>(Model());
	if(model.SubBoundary().Min().Empty() || model.SubBoundary().Max().Empty())
		return true;

	if((model.SubBoundary().Min().Z() > si_depth) || (model.SubBoundary().Max().Z() < si_depth))	
		return true;

	return false;
}


// In
//////////////////////////////////////////////////////////////////////////
bool CTetraSuperHorizonEntry::SurfaceIntersection(const geo::ISurface& surface1, const geo::ISurface& surface2) const
{
	// Do surfaces overlap?
	if(surface1.Min().X() > surface2.Max().X() ||
	   surface1.Min().Y() > surface2.Max().Y() ||
	   surface1.Min().Z() > surface2.Max().Z() ||
	   surface1.Max().X() < surface2.Min().X() ||
	   surface1.Max().Y() < surface2.Min().Y() ||
	   surface1.Max().Z() < surface2.Min().Z())
	return false;

	// Check faces ...
	for(int nFace1 = 0; nFace1 < surface1.FaceSize(); nFace1++)
	{
		const geo::IFace& face1 = surface1.Face(nFace1);
		for(int nFace2 = 0; nFace2 < surface2.FaceSize(); nFace2++)
		{
			const geo::IFace& face2 = surface2.Face(nFace2);
			if(!(face1.Min().X() > face2.Max().X() ||
				 face1.Min().Y() > face2.Max().Y() ||
				 face1.Min().Z() > face2.Max().Z() ||
				 face1.Max().X() < face2.Min().X() ||
				 face1.Max().Y() < face2.Min().Y() ||
				 face1.Max().Z() < face2.Min().Z()))
				 return true;
		}
	}

	return false;
}

bool CTetraSuperHorizonEntry::HorizonIntersection(const geo::ISurface& surface, const C3DHorizon& horizon) const
{
	if(horizon.ConstantDepth())
		return SurfaceIntersection(surface, horizon.GeneratedConstantDepthSurface());
	assert(horizon.SurfaceSize() == 1);
	return SurfaceIntersection(surface, horizon.Surface(0).Surface());
}

CTetraSuperHorizonEntry::CTetraSuperHorizonEntry(CTetraModel& model)
: CStorageNodeEntry<CTetraSuperHorizon>(MD_TETRA_SUPER_HORIZON, IDI_HORIZONS, IDS_TREE_HORIZONS, model)
{
}

bool CTetraSuperHorizonEntry::CanAppendSurface(const geo::ISurface& surface) const
{
	CTetraModel& model = (CTetraModel&)(Model());
	// Check side surface intersection
	for(int i = 0; i < model.SubBoundary().SideSurfaceSize(); i++)
	{
		if(SurfaceIntersection(model.SubBoundary().SideSurface(i).Surface(), surface))
			return false;
	}

	// Checking top horizon
	if(model.SubBoundary().TopHorizon().Horizon())
	{
		if(HorizonIntersection(surface, *model.SubBoundary().TopHorizon().Horizon()))
			return false;
	}

	// Checking lower horizon
	if(model.SubBoundary().BottomHorizon().Horizon())
	{
		if(HorizonIntersection(surface, *model.SubBoundary().TopHorizon().Horizon()))
			return false;
	}

	// Checking horizons of super model
	TNodeSet stNode = EntryNodes();
	for(TNodeSet::iterator it = stNode.begin(); it != stNode.end(); it++)
	{
		if(HorizonIntersection(surface, **it))
			return false;
	}

	return true;
}

bool CTetraSuperHorizonEntry::CanConnectItem(const CGraphNode &item) const
{
	const CTetraSurface* pSurface = dynamic_cast<const CTetraSurface*>(&item);
	if(pSurface)
		return !pSurface->Used();

	return false;
}

bool CTetraSuperHorizonEntry::ConnectItem(const CGraphNode &item)
{
	const CTetraSurface* pSurface = dynamic_cast<const CTetraSurface*>(&item);
	if(pSurface)
	{
		new CTetraSuperHorizon((CTetraSurface&)*pSurface, Model(), false);
		return true;
	}

	return false;
}
