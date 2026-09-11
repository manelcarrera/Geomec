#ifndef _Upscaling_Delegate_h_
#define _Upscaling_Delegate_h_

class IUpscalingNodeBase;
class CUpscalingEntry;
class CUpscalingFormationsBranch;
class IUpscalingPointsetBranchBase;
class CUpscalingPointsetRangesBranch;
class CUpscalingTargetPointsetBranch;

#include "GraphEntry_Delegate.h"
#include "StorageNode_Delegate.h"
#include "OpenGLNode_Delegate.h"

class IUpscalingNodeBase_Delegate : public CStorageNode_Delegate
{
public:
  IUpscalingNodeBase_Delegate(IUpscalingNodeBase* upscalingNodeBase);

  virtual bool Attributes();

  virtual void AppendContextMenu(CContextMenuInvoker& invoker);

  virtual bool CanExecute() const;
  virtual void Execute();

private:
  IUpscalingNodeBase_Delegate(const IUpscalingNodeBase_Delegate& rhs);
  IUpscalingNodeBase_Delegate& operator = (
    const IUpscalingNodeBase_Delegate& rhs);

  IUpscalingNodeBase* m_upscalingNodeBase;

  REGISTER_DELEGATE(IUpscalingNodeBase, IUpscalingNodeBase_Delegate);
};

class CUpscalingEntry_Delegate :
  public CGraphEntryTemp_Delegate <IUpscalingNodeBase>
{
public:
  CUpscalingEntry_Delegate(CUpscalingEntry* upscalingEntry);

  virtual void AppendContextMenu(CContextMenuInvoker& invoker);

private:
  CUpscalingEntry_Delegate(const CUpscalingEntry_Delegate& rhs);
  CUpscalingEntry_Delegate& operator = (const CUpscalingEntry_Delegate& rhs);

  CUpscalingEntry* m_upscalingEntry;

  REGISTER_DELEGATE(CUpscalingEntry, CUpscalingEntry_Delegate);
};

class CUpscalingFormationsBranch_Delegate : public COpenGLNode_Delegate
{
public:
  CUpscalingFormationsBranch_Delegate(
    CUpscalingFormationsBranch* upscalingFormationsBranch);

  virtual bool Attributes();

private:
  CUpscalingFormationsBranch_Delegate(
    const CUpscalingFormationsBranch_Delegate& rhs);
  CUpscalingFormationsBranch_Delegate& operator = (
    const CUpscalingFormationsBranch_Delegate& rhs);

  CUpscalingFormationsBranch* m_upscalingFormationsBranch;

  REGISTER_DELEGATE(CUpscalingFormationsBranch,
    CUpscalingFormationsBranch_Delegate);
};

class IUpscalingPointsetBranchBase_Delegate : public COpenGLNode_Delegate
{
public:
  IUpscalingPointsetBranchBase_Delegate(
    IUpscalingPointsetBranchBase* upscalingPointsetBranchBase);

private:
  IUpscalingPointsetBranchBase_Delegate(
    const IUpscalingPointsetBranchBase_Delegate& rhs);
  IUpscalingPointsetBranchBase_Delegate& operator = (
    const IUpscalingPointsetBranchBase_Delegate& rhs);

  IUpscalingPointsetBranchBase* m_upscalingPointsetBranchBase;

  REGISTER_DELEGATE(IUpscalingPointsetBranchBase,
    IUpscalingPointsetBranchBase_Delegate);
};

class CUpscalingPointsetRangesBranch_Delegate :
  public IUpscalingPointsetBranchBase_Delegate
{
public:
  CUpscalingPointsetRangesBranch_Delegate(
    CUpscalingPointsetRangesBranch* upscalingPointsetRangesBranch);

  virtual bool Attributes();

private:
  CUpscalingPointsetRangesBranch_Delegate(
    const CUpscalingPointsetRangesBranch_Delegate& rhs);
  CUpscalingPointsetRangesBranch_Delegate& operator = (
    const CUpscalingPointsetRangesBranch_Delegate& rhs);

  CUpscalingPointsetRangesBranch* m_upscalingPointsetRangesBranch;

  REGISTER_DELEGATE(CUpscalingPointsetRangesBranch,
    CUpscalingPointsetRangesBranch_Delegate);
};

class CUpscalingTargetPointsetBranch_Delegate : public IUpscalingPointsetBranchBase_Delegate
{
public:
  CUpscalingTargetPointsetBranch_Delegate(
    CUpscalingTargetPointsetBranch* upscalingTargetPointsetBranch);

  virtual void AppendContextMenu(CContextMenuInvoker& invoker);

  bool CanCreateFromRegular2DGrid() const;
  void CreateFromRegular2DGrid();
  bool CanCreateFromExisting2D() const;
  void CreateFromExisting2D();
  bool CanCreateFromSurfaceNodes() const;
  void CreateFromSurfaceNodes();

private:
  CUpscalingTargetPointsetBranch_Delegate(
    const CUpscalingTargetPointsetBranch_Delegate& rhs);
  CUpscalingTargetPointsetBranch_Delegate& operator = (
    const CUpscalingTargetPointsetBranch_Delegate& rhs);

  CUpscalingTargetPointsetBranch* m_upscalingTargetPointsetBranch;

  REGISTER_DELEGATE(CUpscalingTargetPointsetBranch,
    CUpscalingTargetPointsetBranch_Delegate);
};

#endif  // _Upscaling_Delegate_h_
