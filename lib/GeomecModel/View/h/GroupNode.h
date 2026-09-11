#pragma once

#include "openglnode.h"

class CFemAppModel;
class C3DModel;

class CGroupNode : public COpenGLNode
{
public:
  CGroupNode(CFemAppModel& model);
  CGroupNode(const QString &strName, CFemAppModel& model, CGraphEntry *entry);
  CGroupNode(const CGroupNode& rhs);
  virtual ~CGroupNode();

  CGroupNode& operator=(const CGroupNode &rhs);
  bool operator==(const CGroupNode &rhs) const;

  const QString& Description() const;
  void Description(const QString& description);

  bool ReadOnly() const;
  void ReadOnly(bool readOnly);

  virtual bool Empty() const;
  virtual long SavedItems() const;
  virtual void LoadStream(TSTREAM& stream, CStreamVersion& version, TPROGRESS& progress);
  virtual void SaveStream(TSTREAM& stream, TPROGRESS& progress);

  virtual int DisplayListSize() const;
  virtual const geo::IObject& DisplayList(int nIndex) const;
  virtual TColor Color() const;

  virtual bool CanDestroy() const;
  virtual	bool ConnectItem(const CGraphNode &item);
  virtual bool CanConnectItem(const CGraphNode &item) const;
  virtual bool CanDisconnectItem(const CGraphNode& item) const;
 
  virtual void Modified(enum ModifiedHint uHint = Default);

  const CGraphEntry *GraphEntry() const;
  static void RemoveGroups(CGraphNode& graphNode, const CGroupNode *keepGroup = nullptr);

private:
  CGraphEntry *m_entry;

  QString m_description;
  bool m_readOnly;
};

class CGroupNodeEntry : public CStorageNodeEntry<CGroupNode>
{
  typedef CStorageNodeEntry<CGroupNode> BASE_CLASS;

public:
  CGroupNodeEntry(C3DModel &model);
  virtual ~CGroupNodeEntry();
};
