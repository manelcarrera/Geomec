#ifndef _IWellModel_Delegate_h_
#define _IWellModel_Delegate_h_

class IWellModel;

#include "ModelBase_Delegate.h"
#include "BoundaryBase_Delegate.h"
#include "IWellModel.h"
#include "BaseSupportNode_Delegate.h"

class IWellModel_Delegate : public CModelBase_Delegate
{
public:
  class CBoundary_Delegate : public CBoundaryBase_Delegate
  {
  public:
  class IWellSupportNode_Delegate : public CBaseSupportNode_Delegate
  {
  public:
      IWellSupportNode_Delegate(
    IWellModel::CBoundary::IWellSupportNode* wellSupportNode);

  private:
      IWellSupportNode_Delegate(const IWellSupportNode_Delegate& rhs);
      IWellSupportNode_Delegate& operator = (
    const IWellSupportNode_Delegate& rhs);

      IWellModel::CBoundary::IWellSupportNode* m_wellSupportNode;

      REGISTER_DELEGATE(IWellModel::CBoundary::IWellSupportNode,
    IWellSupportNode_Delegate);
  };

  class CDisplacementSupport_Delegate : public IWellSupportNode_Delegate
  {
  public:
      CDisplacementSupport_Delegate(
    IWellModel::CBoundary::CDisplacementSupport* displacementSupport);

  private:
      CDisplacementSupport_Delegate(const CDisplacementSupport_Delegate& rhs);
      CDisplacementSupport_Delegate& operator = (
    const CDisplacementSupport_Delegate& rhs);

      IWellModel::CBoundary::CDisplacementSupport* m_displacementSupport;

      REGISTER_DELEGATE(IWellModel::CBoundary::CDisplacementSupport,
    CDisplacementSupport_Delegate);
  };

  class CPressureSupport_Delegate : public IWellSupportNode_Delegate
  {
  public:
      CPressureSupport_Delegate(
    IWellModel::CBoundary::CPressureSupport* pressureSupport);

  private:
      CPressureSupport_Delegate(const CPressureSupport_Delegate& rhs);
      CPressureSupport_Delegate& operator = (
    const CPressureSupport_Delegate& rhs);

      IWellModel::CBoundary::CPressureSupport* m_pressureSupport;

      REGISTER_DELEGATE(IWellModel::CBoundary::CPressureSupport,
    CPressureSupport_Delegate);
  };

  CBoundary_Delegate(IWellModel::CBoundary* boundary);

  virtual void AppendContextMenu(CContextMenuInvoker& invoker);

  virtual bool CanEdit() const;

  virtual bool CanModify() const;

  void InterfaceAttributes();

  private:
  CBoundary_Delegate(const CBoundary_Delegate& rhs);
  CBoundary_Delegate& operator = (const CBoundary_Delegate& rhs);

  IWellModel::CBoundary* m_boundary;

  REGISTER_DELEGATE(IWellModel::CBoundary, CBoundary_Delegate);
  };

  IWellModel_Delegate(IWellModel* wellModel);

  virtual bool Attributes();

private:
  IWellModel_Delegate(const IWellModel_Delegate& rhs);
  IWellModel_Delegate& operator = (const IWellModel_Delegate& rhs);

  IWellModel* m_wellModel;

  REGISTER_DELEGATE(IWellModel, IWellModel_Delegate);
};

#endif  // _IWellModel_Delegate_h_
