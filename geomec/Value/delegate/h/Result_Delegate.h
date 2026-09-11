#ifndef _Result_Delegate_h_
#define _Result_Delegate_h_

class CResultGroup;
class IResult;
class IResultComponent;
class IScalarResultComponent;

#include "GraphNode_Delegate.h"
#include "IValueComponent_Delegate.h"
#include "IValueComposite_Delegate.h"

class CResultGroup_Delegate : public CGraphNode_Delegate {
public:
  CResultGroup_Delegate(CResultGroup *resultGroup);

  virtual void AppendContextMenu(CContextMenuInvoker &invoker);

  bool CanImport() const;
  void Import();

  bool CanExport() const;
  void Export();

  bool CanExportGocad() const;
  void ExportGocad();

private:
  CResultGroup_Delegate(const CResultGroup_Delegate &rhs);
  CResultGroup_Delegate &operator=(const CResultGroup_Delegate &rhs);

  CResultGroup *m_resultGroup;

  REGISTER_DELEGATE(CResultGroup, CResultGroup_Delegate);
};

class IResult_Delegate : public IValueComposite_Delegate {
public:
  IResult_Delegate(IResult *result);

  virtual void AppendContextMenu(CContextMenuInvoker &invoker);

  bool CanExport() const;
  void Export();

private:
  IResult_Delegate(const IResult_Delegate &rhs);
  IResult_Delegate &operator=(const IResult_Delegate &rhs);

  IResult *m_result;

  REGISTER_DELEGATE(IResult, IResult_Delegate);
};

class IResultComponent_Delegate : public IValueComponent_Delegate {
public:
  IResultComponent_Delegate(IResultComponent *resultComponent);

  virtual void AppendContextMenu(CContextMenuInvoker &invoker);

  bool CanExport() const;
  void Export();

private:
  IResultComponent_Delegate(const IResultComponent_Delegate &rhs);
  IResultComponent_Delegate &operator=(const IResultComponent_Delegate &rhs);

  IResultComponent *m_resultComponent;

  REGISTER_DELEGATE(IResultComponent, IResultComponent_Delegate);
};

class IScalarResultComponent_Delegate : public IResultComponent_Delegate {
public:
  IScalarResultComponent_Delegate(IScalarResultComponent *scalarResultComponent);

private:
  IScalarResultComponent_Delegate(const IScalarResultComponent_Delegate &rhs);
  IScalarResultComponent_Delegate &operator=(const IScalarResultComponent_Delegate &rhs);

  IScalarResultComponent *m_scalarResultComponent;

  REGISTER_DELEGATE(IScalarResultComponent, IScalarResultComponent_Delegate);
};

#endif // _Result_Delegate_h_
