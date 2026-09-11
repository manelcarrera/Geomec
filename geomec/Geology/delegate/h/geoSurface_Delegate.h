#ifndef _geoSurface_Delegate_h_
#define _geoSurface_Delegate_h_

class CSurfaceBase;
class CSurfaceEntry;

#include "StorageNode_Delegate.h"
#include "ElementSet_Delegate.h"
#include "TriaIntersecBox.h"

class CSurfaceBase_Delegate : public IElementSet_Delegate
{
public:
  CSurfaceBase_Delegate(CSurfaceBase* surfaceBase);

  virtual bool Attributes();

  virtual void AppendContextMenu(CContextMenuInvoker& invoker);

  virtual bool CanDestroy() const;

  void ExportSurface();

private:
  CSurfaceBase_Delegate(const CSurfaceBase_Delegate& rhs);
  CSurfaceBase_Delegate& operator = (const CSurfaceBase_Delegate& rhs);

  CSurfaceBase* m_surfaceBase;

  REGISTER_DELEGATE(CSurfaceBase, CSurfaceBase_Delegate);
};

typedef std::vector <STNamedSurface> CNamedSurfaces;

class CSurfaceEntry_Delegate : public CStorageNodeEntry_Delegate <CSurfaceBase>
{
public:
  CSurfaceEntry_Delegate(CSurfaceEntry* surfaceEntry);

  virtual void AppendContextMenu(CContextMenuInvoker& invoker);

  bool CanImport() const;
  void Import();

  bool GetSurfaceCount() const;
  void ExportSurfaces();

  bool NonUsedSurface() const;
  void DoSelectAndCreateHorizons();

  bool CanSelectAndCreateSubBoundarySides() const;
  void DoSelectAndCreateSubBoundarySides();

  bool CanCreateSubBoundaryTopAndBottom() const;
  void DoCreateSubBoundaryTopAndBottom();

  bool CanSelectAndCreateFaults() const;
  void DoSelectAndCreateFaults();

  bool CanSelectAndUnassignSurfaces() const;
  void DoSelectAndUnassignSurfaces();

  bool CanSelectAndDeleteSurfaces() const;
  void DoSelectAndDeleteSurfaces();

  bool CanMergeFaultSurfaces() const;
  void DoMergeFaultSurfaces();

  void CutSurfaces();

  void MergeSurfaces();

  void CutSurfacesBB(const geo::CBox& box);

private:
  CSurfaceEntry_Delegate(const CSurfaceEntry_Delegate& rhs);
  CSurfaceEntry_Delegate& operator = (const CSurfaceEntry_Delegate& rhs);

  void DoCutSurfaces(const geo::CBox* pbox = 0);

  enum eCheckUsed
  {
  NO_CHECK,
  USED_IN_MODEL,
  NOT_USED_IN_MODEL
  };
  void SelectSurfaces(CNamedSurfaces&, eCheckUsed check, const char *caption = 0);

  CSurfaceEntry* m_surfaceEntry;

  REGISTER_DELEGATE(CSurfaceEntry, CSurfaceEntry_Delegate);
};

#endif  // _geoSurface_Delegate_h_
