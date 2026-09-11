#ifndef _GoCadModel_Delegate_h_
#define _GoCadModel_Delegate_h_

class CGoCadSubModelEntry;
class CGoCadModel;

#include "GraphEntry_Delegate.h"
#include "TetraModel_Delegate.h"

class CGoCadModel_Delegate : public CTetraModel_Delegate
{
public:
  CGoCadModel_Delegate(CGoCadModel* aGoCadModel);

private:
  CGoCadModel_Delegate(const CGoCadModel_Delegate& rhs);
  CGoCadModel_Delegate& operator = (const CGoCadModel_Delegate& rhs);

  CGoCadModel* m_GoCadModel;

  REGISTER_DELEGATE(CGoCadModel, CGoCadModel_Delegate);
};

typedef CGraphEntryTemp_Delegate <COpenGLNode> TSubModelEntry_Delegate;

class CGoCadSubModelEntry_Delegate : public TSubModelEntry_Delegate
{
public:
  typedef CSingleCommandTemplate <CGoCadSubModelEntry_Delegate> TEntryCommand;

  CGoCadSubModelEntry_Delegate(CGoCadSubModelEntry* goCadSubModelEntry);

  virtual void AppendContextMenu(CContextMenuInvoker& invoker);

  bool CanImportMesh() const;
  void ImportMesh();
  void ImportMesh(const QString& fileName);

  bool CanInsertSurfaceContainer() const; 
  void InsertFault();
  void InsertHorizon();

private:
  CGoCadSubModelEntry_Delegate(const CGoCadSubModelEntry_Delegate& rhs);
  CGoCadSubModelEntry_Delegate& operator = (
    const CGoCadSubModelEntry_Delegate& rhs);

  CGoCadSubModelEntry* m_goCadSubModelEntry;

  REGISTER_DELEGATE(CGoCadSubModelEntry, CGoCadSubModelEntry_Delegate);
};

#endif  // _GoCadModel_Delegate_h_
