// TetraModelObserver.cpp: implementation of the CTetraModel class.
//
//////////////////////////////////////////////////////////////////////

#include "TetraModelObserver.h"
#include "3DHorizon.h"
#include "3DHorizonObserver.h"
#include "AnalysisPoint.h"
#include "ColorScaleEntry.h"
#include "CrossSection.h"
#include "DerivedResult.h"
#include "FaultPressure.h"
#include "GeoSurface.h"
#include "Geomec.h"
#include "GeomecDianaRunner.h"
#include "GlobalInitialStressNode.h"
#include "GlobalMessage.h"
#include "GlobalPressure.h"
#include "HomogenizationBox.h"
#include "KeyFile.h"
#include "MeasuredTopDisplacementsNode_Delegate.h"
#include "MeshBase.h"
#include "ResultRegister.h"
#include "ResultTree.h"
#include "TetraBoundary.h"
#include "TetraEntryTypes.h"
#include "TetraFormation.h"
#include "TetraMesh.h"
#include "TetraModel.h"
#include "TetraSubBoundary.h"
#include "TetraSubBoundaryObserver.h"
#include "TetraSuperHorizon.h"
#include "geoSurface_Delegate.h"
#include "materialentry.h"
#include "stdafx.h"

#ifdef _DEBUG
#ifdef _MSC_VER
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif // _MSC_VER
// #define new DEBUG_NEW
#endif

//=============================================================================
CTetraSubModelBranch::CTetraSubModelBranch(ITreeObject &parent, CTetraModel &model)
    : TTetraSubModelBranch((TSubModelEntry &)(*model.GraphEntry(MD_TETRA_SUB_ALL)), parent.Ctrl(), FALSE, FIXED_ITEM,
                           parent.Handle(), TVI_LAST, FALSE, FALSE),
      m_model(model) {
  Ctrl().SetItemData(Handle(), (DWORD_PTR)(this));

  // Create branches
  typedef COpenGLNodeObserver_Delegate<CSurfaceBase, CSurfaceBase_Delegate, CDummyNode, CDummyObserver, FALSE,
                                       FIXED_ITEM>
      TSurfaceObserver;
  typedef COpenGLNodeObserver_Delegate<CTetraSubHorizon, CTetraSubHorizon_Delegate, CSurfaceBase, TSurfaceObserver,
                                       TRUE, UNLINK_ITEM>
      TSubHorizonObs;
  //	typedef CStateBranch_Delegate<CTetraSubHorizonEntry, CTetraSubHorizonEntry_Delegate, CHorizonBase,
  // CHorizonObserver<FALSE>, CUndefinedIconProvider, TRUE, DELETE_ITEM> TSubHorizonEntryObs;
  typedef C3DHorizonEntryObserver<false> TSubHorizonEntryObs_local;

#ifdef TETRA_FAULTS_KEYFILE
  const CKeyFile &keyfile = (dynamic_cast<const CGeomecApp *>(AfxGetApp()))->KeyFile();
  if (keyfile.Unlocked()) {
#endif
    //		new TSubHorizonEntryObs((CTetraSubHorizonEntry&)(*model.GraphEntry(MD_TETRA_SUB_FAULT)),
    //								Ctrl(),
    //								TRUE,
    //								FIXED_ITEM,
    //								Handle());
    new TSubHorizonEntryObs_local(*model.GraphEntry(MD_TETRA_SUB_FAULT), Ctrl(), TRUE, FIXED_ITEM, Handle());
#ifdef TETRA_FAULTS_KEYFILE
  }
#endif

  // Horizons
  //	new TSubHorizonEntryObs((CTetraSubHorizonEntry&)(*model.GraphEntry(MD_TETRA_SUB_HORIZON)),
  //							Ctrl(),
  //							TRUE,
  //							FIXED_ITEM,
  //							Handle());
  new TSubHorizonEntryObs_local(*model.GraphEntry(MD_TETRA_SUB_HORIZON), Ctrl(), TRUE, FIXED_ITEM, Handle());
  //	new CSubHorizonEntryObserver(*this, model);
  //	new CSubFaultEntryObserver(*this, model);
  //	CTetraSubBoundary& boundary = dynamic_cast<CTetraSubBoundary&>(*model.GraphEntry(MD_TETRA_SUB_BOUNDARY));
  new CTetraSubBoundaryObserver(*this, model.SubBoundary());
}
