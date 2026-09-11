#include "stdafx.h"

#include "Upscaling_Delegate.h"
#include "ModelBase.h"  // Mandatory order, before "Upscaling.h"
#include "Upscaling.h"
#include "RenameNodeDlg.h"
#include "SelectFormationsDlg.h"
#include "AttriThinLayerUpscalingTargetPointsets.h"
#include "ContextMenuInvoker.h"
#include "ThinLayerUpscalingCreateTargetPointset.h"
#include "PointSet.h"
#include "GeoSurface.h"

IUpscalingNodeBase_Delegate::IUpscalingNodeBase_Delegate(
  IUpscalingNodeBase* upscalingNodeBase)
: CStorageNode_Delegate(upscalingNodeBase)
, m_upscalingNodeBase(upscalingNodeBase)
{
}

bool IUpscalingNodeBase_Delegate::Attributes()
{
  CRenameNodeDlg dlg(*m_upscalingNodeBase);

  return dlg.DoModal() == IDOK;
}

void IUpscalingNodeBase_Delegate::AppendContextMenu(
  CContextMenuInvoker& invoker)
{
  typedef CSingleCommandTemplate <IUpscalingNodeBase_Delegate> TCommand;

  invoker.AddCommand("Execute",
  *new TCommand(*this, &IUpscalingNodeBase_Delegate::Execute,
      &IUpscalingNodeBase_Delegate::CanExecute));
  invoker.AddSeparator();

  CStorageNode_Delegate::AppendContextMenu(invoker);
}

bool IUpscalingNodeBase_Delegate::CanExecute() const
{
  assert(false);

  return false;
}

void IUpscalingNodeBase_Delegate::Execute()
{
  assert(false);
}

CUpscalingEntry_Delegate::CUpscalingEntry_Delegate(
  CUpscalingEntry* upscalingEntry)
: CGraphEntryTemp_Delegate <IUpscalingNodeBase> (upscalingEntry)
, m_upscalingEntry(upscalingEntry)
{
}

void CUpscalingEntry_Delegate::AppendContextMenu(CContextMenuInvoker& invoker)
{
  typedef CSingleCommandTemplate <CUpscalingEntry> TCommand;

  invoker.AddCommand("New Thin Layer Upscaling",
  *new TCommand(*m_upscalingEntry,
      &CUpscalingEntry::CreateThinLayerUpscaling));
  invoker.AddCommand("New CamClay Upscaling",
  *new TCommand(*m_upscalingEntry,
      &CUpscalingEntry::CreateCamClayUpscaling));

  CGraphEntryTemp_Delegate <IUpscalingNodeBase> ::AppendContextMenu(invoker);
}

CUpscalingFormationsBranch_Delegate::CUpscalingFormationsBranch_Delegate(
  CUpscalingFormationsBranch* upscalingFormationsBranch)
: COpenGLNode_Delegate(upscalingFormationsBranch)
, m_upscalingFormationsBranch(upscalingFormationsBranch)
{
}

bool CUpscalingFormationsBranch_Delegate::Attributes()
{
  CSelectFormationsDlg dlg(*m_upscalingFormationsBranch,
  m_upscalingFormationsBranch->FormationFilter());

  return (dlg.DoModal() == IDOK);
}

IUpscalingPointsetBranchBase_Delegate::IUpscalingPointsetBranchBase_Delegate(
  IUpscalingPointsetBranchBase* upscalingPointsetBranchBase)
: COpenGLNode_Delegate(upscalingPointsetBranchBase)
, m_upscalingPointsetBranchBase(upscalingPointsetBranchBase)
{
}

CUpscalingPointsetRangesBranch_Delegate::
  CUpscalingPointsetRangesBranch_Delegate(
  CUpscalingPointsetRangesBranch* upscalingPointsetRangesBranch)
: IUpscalingPointsetBranchBase_Delegate(upscalingPointsetRangesBranch)
, m_upscalingPointsetRangesBranch(upscalingPointsetRangesBranch)
{
}

bool CUpscalingPointsetRangesBranch_Delegate::Attributes()
{
  CAttriUpscalingTargetPointsets dlg(*m_upscalingPointsetRangesBranch);

  return dlg.DoModal() == IDOK;
}

CUpscalingTargetPointsetBranch_Delegate::
  CUpscalingTargetPointsetBranch_Delegate(
  CUpscalingTargetPointsetBranch* upscalingTargetPointsetBranch)
: IUpscalingPointsetBranchBase_Delegate(upscalingTargetPointsetBranch)
, m_upscalingTargetPointsetBranch(upscalingTargetPointsetBranch)
{
}

void CUpscalingTargetPointsetBranch_Delegate::AppendContextMenu(
  CContextMenuInvoker& invoker)
{
  typedef CSingleCommandTemplate <CUpscalingTargetPointsetBranch> TCommand;
  typedef CSingleCommandTemplate <CUpscalingTargetPointsetBranch_Delegate>
  TCommand_Delegate;

  CContextMenuInvoker* pSubMenu = new CContextMenuInvoker;

  pSubMenu->AddCommand("Define Regular 2D Grid",
  *new TCommand_Delegate(*this,
      &CUpscalingTargetPointsetBranch_Delegate::CreateFromRegular2DGrid,
      &CUpscalingTargetPointsetBranch_Delegate::CanCreateFromRegular2DGrid));
  pSubMenu->AddCommand("From Existing 2D",
  *new TCommand_Delegate(*this,
      &CUpscalingTargetPointsetBranch_Delegate::CreateFromExisting2D,
      &CUpscalingTargetPointsetBranch_Delegate::CanCreateFromExisting2D));
  pSubMenu->AddCommand("From Surface Nodes",
  *new TCommand_Delegate(*this,
      &CUpscalingTargetPointsetBranch_Delegate::CreateFromSurfaceNodes,
      &CUpscalingTargetPointsetBranch_Delegate::CanCreateFromSurfaceNodes));
  pSubMenu->AddCommand("From Hexa Midpoints",
  *new TCommand(*m_upscalingTargetPointsetBranch,
      &CUpscalingTargetPointsetBranch::CreateFromHexaMidpoints,
      &CUpscalingTargetPointsetBranch::CanCreateFromHexaMidpoints));

  invoker.AddInvoker("Generate 3D pointset", *pSubMenu);
}

bool CUpscalingTargetPointsetBranch_Delegate::CanCreateFromRegular2DGrid() const
{
  return m_upscalingTargetPointsetBranch->CanCreateFromRegular2DGrid();
}

void CUpscalingTargetPointsetBranch_Delegate::CreateFromRegular2DGrid()
{
  CDefineRegular2DGridDlg dlg(static_cast <CModelBase&> (
  m_upscalingTargetPointsetBranch->Model()));

  if (dlg.DoModal() == IDOK)
  {
  m_upscalingTargetPointsetBranch->CreateFromRegular2DGrid(dlg.Corner1(),
      dlg.Corner2(), dlg.NumPointsNorthing(), dlg.NumPointsEasting());
  }
}

bool CUpscalingTargetPointsetBranch_Delegate::CanCreateFromExisting2D() const
{
  return m_upscalingTargetPointsetBranch->CanCreateFromExisting2D();
}

void CUpscalingTargetPointsetBranch_Delegate::CreateFromExisting2D()
{
  CSelect2DPointsetDlg dlg(static_cast <CModelBase&> (
  m_upscalingTargetPointsetBranch->Model()));

  if (dlg.DoModal() == IDOK)
  {
  assert(dlg.SelectedPointset());
  m_upscalingTargetPointsetBranch->CreateFromExisting2D(
      *dlg.SelectedPointset());
  }
}

bool CUpscalingTargetPointsetBranch_Delegate::CanCreateFromSurfaceNodes() const
{
  return m_upscalingTargetPointsetBranch->CanCreateFromSurfaceNodes();
}

void CUpscalingTargetPointsetBranch_Delegate::CreateFromSurfaceNodes()
{
  CSelectSurfaceDlg dlg(static_cast <CModelBase&> (
  m_upscalingTargetPointsetBranch->Model()));

  if (dlg.DoModal() == IDOK)
  {
  assert(dlg.SelectedSurface());
  m_upscalingTargetPointsetBranch->CreateFromSurfaceNodes(
      *dlg.SelectedSurface());
  }
}
