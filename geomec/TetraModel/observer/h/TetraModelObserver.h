// TetraModelObserver.h: interface for the CTetraModel class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TETRAMODELOBSERVER_H__7447A784_3512_4259_B32F_E2A3875222CF__INCLUDED_)
#define AFX_TETRAMODELOBSERVER_H__7447A784_3512_4259_B32F_E2A3875222CF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "3dmodel.h"
#include "TetraSubHorizon.h"
#include "TetraSuperHorizon.h"
#include "OpenGLNodeObserver.h"

class CTetraMesh;
class CTetraSubBoundary;

#include "TetraSubHorizon_Delegate.h"
#include "StateBranch_Delegate.h"
#include "UndefinedIconProvider.h"

// Sub model entry contains :
// - All faults in sub model
// - All horizons in sub model
// - The boundary in the sub model

typedef CGraphEntryTemp_Delegate<COpenGLNode> TSubModelEntry_Delegate;

typedef CNodeObserver_Delegate<TSubModelEntry, TSubModelEntry_Delegate, CDummyNode, CDummyObserver, FALSE, FIXED_ITEM> TTetraSubModelBranch;
class CTetraSubModelBranch : public TTetraSubModelBranch
{
	typedef COpenGLNodeObserver_Delegate<CSurfaceBase, CSurfaceBase_Delegate, CDummyNode, CDummyObserver, FALSE, FIXED_ITEM> TSurfaceObserver;
	typedef COpenGLNodeObserver_Delegate<CTetraSubHorizon, CTetraSubHorizon_Delegate, CSurfaceBase, TSurfaceObserver, TRUE, UNLINK_ITEM> TSubHorizonObs;
	typedef CStateBranch_Delegate<CTetraSubHorizonEntry, CTetraSubHorizonEntry_Delegate, CTetraSubHorizon, TSubHorizonObs, CUndefinedIconProvider, TRUE, DELETE_ITEM> TSubHorizonEntryObs;

	CTetraModel& m_model;
public:
	CTetraSubModelBranch(ITreeObject& parent, CTetraModel& model);
};


#endif // !defined(AFX_TETRAMODELOBSERVER_H__7447A784_3512_4259_B32F_E2A3875222CF__INCLUDED_)
