#include "stdafx.h"

#include "TetraSurface_Delegate.h"
#include "TetraSurface.h"
#include "ContextMenuInvoker.h"
#include "KeyFile.h"
#include "Geomec.h"
#include "GeomecDoc.h"
#include "TetraSubHorizon.h"
#include "HexaModel.h"
#include "HexaHorizon.h"
#include "TetraModel.h"
#include "TetraSubBoundary.h"
#include "TetraSuperHorizon.h"

CTetraSurface_Delegate::CTetraSurface_Delegate(CTetraSurface* tetraSurface)
: CSurfaceBase_Delegate(tetraSurface)
, m_tetraSurface(tetraSurface)
{
}

void CTetraSurface_Delegate::CreateSideSurface()
{
  CTetraModel *model = static_cast<CTetraModel *>(GetGeomecDoc()->Model());
  CTetraSubBoundary &sub = model->SubBoundary();
  sub.LinkTo(*m_tetraSurface);
  model->InvalidateMesh();
}

void CTetraSurface_Delegate::CreateSuperHorizon()
{
  CTetraModel *model = static_cast<CTetraModel *>(GetGeomecDoc()->Model());
  new CTetraSuperHorizon(*m_tetraSurface, *model, false);
  model->InvalidateMesh();
}

void CTetraSurface_Delegate::CreateSubHorizon()
{
  CModelBase *model = static_cast<CModelBase *>(GetGeomecDoc()->Model());
  if (dynamic_cast<CHexaModel *>(model))
  {
  new CHexaHorizon(*m_tetraSurface, *model, false);
  }
  else
  {
  new CTetraSubHorizon(*m_tetraSurface, *model, false);
  }
  model->InvalidateMesh();
}

void CTetraSurface_Delegate::CreateSubFault()
{
  CTetraModel *model = static_cast<CTetraModel *>(GetGeomecDoc()->Model());
  new CTetraSubHorizon(*m_tetraSurface, *model, true);
  model->InvalidateMesh();
}

void CTetraSurface_Delegate::AppendContextMenu(CContextMenuInvoker &invoker)
{
  if (!m_tetraSurface->Used())
  {
  CModelBase *model = static_cast<CModelBase *>(GetGeomecDoc()->Model());
  bool bIsHexa = dynamic_cast<CHexaModel *>(model) != 0;

  const CKeyFile& keyfile =
      (dynamic_cast <const CGeomecApp*> (AfxGetApp()))->KeyFile();

  if (!bIsHexa)
  {
      invoker.AddCommand(_T("Insert sidesurface in sub model"),
    *(new TTetraSurfaceDelegateCommand(*this,
    &CTetraSurface_Delegate::CreateSideSurface)));
      invoker.AddSeparator();
  }
  invoker.AddCommand(_T("Insert horizon in sub model"),
      *(new TTetraSurfaceDelegateCommand(*this,
      &CTetraSurface_Delegate::CreateSubHorizon)));

#ifdef TETRA_SUPERMODEL_KEYFILE
  if (keyfile.Unlocked())
  {
#endif
      invoker.AddCommand(_T("Insert horizon in super model"),
    *(new CTetraSurface::TTetraSurfaceCommand(*m_tetraSurface,
    &CTetraSurface::CreateSuperHorizon)));
#ifdef TETRA_SUPERMODEL_KEYFILE
  }
#endif

#ifdef TETRA_FAULTS_KEYFILE
  if(keyfile.Unlocked())
  {
#endif
      if (!bIsHexa)
      {
    invoker.AddSeparator();
    invoker.AddCommand(_T("Insert fault in sub model"),
          *(new TTetraSurfaceDelegateCommand(*this,
          &CTetraSurface_Delegate::CreateSubFault)));
      }
#ifdef TETRA_FAULTS_KEYFILE
  }
#endif
  invoker.AddSeparator();
  }

  CSurfaceBase_Delegate::AppendContextMenu(invoker);
}
