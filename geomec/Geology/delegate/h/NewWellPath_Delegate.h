#ifndef _NewWellPath_Delegate_h_
#define _NewWellPath_Delegate_h_

class CNewWellPath;
class CNewWellPathEntry;

#include "ColorNode_Delegate.h"

class CNewWellPath_Delegate : public CColorNode_Delegate
{
public:
  CNewWellPath_Delegate(CNewWellPath* newWellPath);

  virtual void AppendContextMenu(CContextMenuInvoker& invoker);

  virtual bool Edit();
  virtual bool Attributes();

  bool CanExportToFist() const;
  void ExportToFist();
  bool CanDefineFistPoints() const;
  void DefineFistPoints();
  void CreateZoomInModel();
  bool CanZoomIn() const;
  void ExportLAS();
  bool CanExportLAS() const;
  void ImportLAS();
  bool CanImportLAS() const;
  void GenerateIDMInput();

  void FlipDepth();

private:
  CNewWellPath_Delegate(const CNewWellPath_Delegate& rhs);
  CNewWellPath_Delegate& operator = (const CNewWellPath_Delegate& rhs);

  CNewWellPath* m_pNewWellPath;

  REGISTER_DELEGATE(CNewWellPath, CNewWellPath_Delegate);
};

class CNewWellPathEntry_Delegate :
  public CStorageNodeEntry_Delegate <CNewWellPath>
{
public:
  CNewWellPathEntry_Delegate(CNewWellPathEntry* newWellPathEntry);

  virtual void AppendContextMenu(CContextMenuInvoker& invoker);

  virtual void InsertGroup();

  bool CanExportToFist() const;
  void ExportToFist();

  bool CanInsertNew() const;
  void InsertDeviatedWellPathByPointSet();
  void InsertVerticalWellPathByValue();
  void InsertVerticalWellPathGraphical();
  void EditVerticalWellPath(CNewWellPath* pWellPath = 0);


  bool CanExport() const;
  void Export();

private:
  CNewWellPathEntry_Delegate(const CNewWellPathEntry_Delegate& rhs);
  CNewWellPathEntry_Delegate& operator =
  (const CNewWellPathEntry_Delegate& rhs);

  CNewWellPathEntry* m_newWellPathEntry;

  REGISTER_DELEGATE(CNewWellPathEntry, CNewWellPathEntry_Delegate);
};

#endif  // _NewWellPath_Delegate_h_
