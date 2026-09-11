#ifndef _HomogenizationBox_Delegate_h_
#define _HomogenizationBox_Delegate_h_

class CHomoBoxEntry;

#include "StorageNode_Delegate.h"
#include "IScalingBox_Delegate.h"
#include "IValueComposite_Delegate.h"
#include "HomogenizationBox.h"

class CHomogenizationBox_Delegate : public IScalingBox_Delegate
{
public:
  CHomogenizationBox_Delegate(CHomogenizationBox* homogenizationBox);

  virtual bool Attributes();

  virtual void AppendContextMenu(CContextMenuInvoker& invoker);

  void CaptureElSet();
  bool CanCaptureElSet() const;

  virtual bool Destroy();

  void CreateZoomIn();

  bool CanCutSurafceInDatastoreWithBox() const;
  void CutSurafceInDatastoreWithBox();

private:
  CHomogenizationBox_Delegate(const CHomogenizationBox_Delegate& rhs);
  CHomogenizationBox_Delegate& operator = (
  const CHomogenizationBox_Delegate& rhs);

  CHomogenizationBox* m_homogenizationBox;

  REGISTER_DELEGATE(CHomogenizationBox, CHomogenizationBox_Delegate);
};

class CHomoBoxEntry_Delegate :
  public CStorageNodeEntry_Delegate <CHomogenizationBox>
{
public:
  CHomoBoxEntry_Delegate(CHomoBoxEntry* homoBoxEntry);

  virtual void AppendContextMenu(CContextMenuInvoker& invoker);

  void NewBox();

private:
  CHomoBoxEntry_Delegate(const CHomoBoxEntry_Delegate& rhs);
  CHomoBoxEntry_Delegate& operator = (const CHomoBoxEntry_Delegate& rhs);

  CHomoBoxEntry* m_homoBoxEntry;

  REGISTER_DELEGATE(CHomoBoxEntry, CHomoBoxEntry_Delegate);
};

#endif  // _HomogenizationBox_Delegate_h_
