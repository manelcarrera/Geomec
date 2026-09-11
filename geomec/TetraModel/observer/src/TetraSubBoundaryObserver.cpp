#include "stdafx.h"

#include "TetraSubBoundaryObserver.h"
#include "OpenGLNodeObserver.h"
#include "TetraModel.h"
#include "BranchState.h"
#include "ContextMenuInvoker.h"
#include "GeoSurfaceObserver.h"
#include "TetraModelObserver.h"
#include "EnumerationBranch.h"

typedef COpenGLNodeObserver_Delegate<CSurfaceBase, CSurfaceBase_Delegate, CDummyNode, CDummyObserver, FALSE, FIXED_ITEM> TSurfObs;
typedef CEnumerationBranch<CSurfaceBase, TSurfObs, TRUE, UNLINK_ITEM> TSurfaceEnumerator;

class CSideSurfaceObs : public TSurfaceEnumerator
{
public:
	CSideSurfaceObs(CTetraSubBoundaryObserver& obs)
		:TSurfaceEnumerator(obs, "Side surfaces", IDI_SIDE_SURFACES)
	{

	}

	void DoRemoveAllSurfaces()
	{
		
		while(Children().size())
		{
			TChildren items = Children();
			TChildren::iterator it = items.begin();  
			TSurfaceObs* pTreeObj = dynamic_cast<TSurfaceObs*>(*it);
			assert(pTreeObj);
			assert(pTreeObj->CanDelete());
			pTreeObj->Delete();
		}
	}

	bool CanRemoveAllSurfaces() const
	{
		if(Children().size())
		{
			TSurfaceObs* pTreeObj = dynamic_cast<TSurfaceObs*>(Children()[0]);
      if(!pTreeObj->CanDelete())
        return false;

			CSurfaceBase& surf = static_cast<CSurfaceBase&>(pTreeObj->ObservedItem());
			const CModelBase& model = static_cast<const CModelBase&>(surf.Model());
			if(model.BranchState().IsBranch())
				return false;

			return true;
		}

		return false;
	}


	virtual void AppendContextMenu(CContextMenuInvoker &invoker)
	{
		//TSideSurfaceObs::AppendContextMenu(invoker);
		//CGeomecApp *pApp = (CGeomecApp *)(AfxGetApp());
		typedef CSingleCommandTemplate<CSideSurfaceObs> TEntryCommand;
		invoker.AddCommand(_T("Delete all side surfaces"),*(new TEntryCommand(*this,&CSideSurfaceObs::DoRemoveAllSurfaces,&CSideSurfaceObs::CanRemoveAllSurfaces)));
	}

	virtual BOOL OnSelect()
	{
		CTetraSubBoundary& subboundary = dynamic_cast<CTetraSubBoundary&>(ObservedItem());
		subboundary.setDrawMode(CTetraSubBoundary::DRAW_SIDES);

		return TSurfaceEnumerator::OnSelect();
	}

};


CTetraSubBoundaryObserver::CTetraSubBoundaryObserver(CTetraSubModelBranch& model_branch, CTetraSubBoundary& boundary)
: TTetraSubBoundaryObserver(boundary, model_branch, FALSE, FIXED_ITEM, TVI_LAST, FALSE, FALSE)
{
	// Surface observer

	// Horizon observer
	typedef COpenGLNodeObserver_Delegate<CTetraSubHorizon, CTetraSubHorizon_Delegate, CDummyNode, CDummyObserver, FALSE, FIXED_ITEM>THorizonObs;
	typedef CNodeObserver_Delegate<CTetraSubBoundary::CHorizonPlaceHolder, CTetraSubBoundary_Delegate::CHorizonPlaceHolder_Delegate, CTetraSubHorizon, THorizonObs, FALSE, UNLINK_ITEM> TPlaceHldrObs;
	Ctrl().SetItemData(Handle(), (DWORD_PTR)(this));

	new TPlaceHldrObs(boundary.TopHorizon(), *this, FALSE, FIXED_ITEM, TVI_LAST, TRUE, FALSE);
	//new TSurfaceEnumerator(*this, "Side surfaces", IDI_SIDE_SURFACES);
	new CSideSurfaceObs(*this);
	new TPlaceHldrObs(boundary.BottomHorizon(), *this, FALSE, FIXED_ITEM, TVI_LAST, TRUE, FALSE);
	SortChildren();
}

BOOL CTetraSubBoundaryObserver::OnSelect()
{
	CTetraSubBoundary& subboundary = dynamic_cast<CTetraSubBoundary&>(ObservedItem());
	subboundary.setDrawMode(CTetraSubBoundary::DRAW_ALL);

	return TTetraSubBoundaryObserver::OnSelect();
}



