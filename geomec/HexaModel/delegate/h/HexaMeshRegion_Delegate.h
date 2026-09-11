#ifndef _HexaMeshRegion_Delegate_h_
#define _HexaMeshRegion_Delegate_h_

#include "hexameshregion.h"
#include "GraphEntry_Delegate.h"
#include "MeshRegionBase_Delegate.h"

class CHexaMeshRegionBase_Delegate : public CMeshRegionBase_Delegate
{
public:
  CHexaMeshRegionBase_Delegate(CHexaMeshRegionBase* hexaMeshRegionBase);

  virtual void AppendContextMenu(CContextMenuInvoker& invoker);

  virtual bool CanEdit() const;

  virtual bool CanDestroy() const;
  virtual bool Destroy();

private:
  CHexaMeshRegionBase_Delegate(const CHexaMeshRegionBase_Delegate& rhs);
  CHexaMeshRegionBase_Delegate& operator = (
    const CHexaMeshRegionBase_Delegate& rhs);

  CHexaMeshRegionBase* m_hexaMeshRegionBase;

  REGISTER_DELEGATE(CHexaMeshRegionBase, CHexaMeshRegionBase_Delegate);
};

class CHexaMainMeshRegion_Delegate : public CHexaMeshRegionBase_Delegate
{
public:
  CHexaMainMeshRegion_Delegate(CHexaMainMeshRegion* hexaMainMeshRegion);

  virtual bool Attributes();

  virtual bool Edit();

  virtual bool CanDestroy() const;

private:
  CHexaMainMeshRegion_Delegate(const CHexaMainMeshRegion_Delegate& rhs);
  CHexaMainMeshRegion_Delegate& operator = (
    const CHexaMainMeshRegion_Delegate& rhs);

  CHexaMainMeshRegion* m_hexaMainMeshRegion;

  REGISTER_DELEGATE(CHexaMainMeshRegion, CHexaMainMeshRegion_Delegate);
};

class CHexaSubMeshRegion_Delegate : public CHexaMeshRegionBase_Delegate
{
public:
  CHexaSubMeshRegion_Delegate(CHexaSubMeshRegion* hexaSubMeshRegion);

  virtual bool Attributes();

  virtual bool Edit();

  bool PropertiesOK();

private:
  CHexaSubMeshRegion_Delegate(const CHexaSubMeshRegion_Delegate& rhs);
  CHexaSubMeshRegion_Delegate& operator = (
    const CHexaSubMeshRegion_Delegate& rhs);

  CHexaSubMeshRegion* m_hexaSubMeshRegion;

  REGISTER_DELEGATE(CHexaSubMeshRegion, CHexaSubMeshRegion_Delegate);
};

class CHexaMeshRegionEntry_Delegate :
  public CGraphEntryTemp_Delegate <CHexaMeshRegionBase>
{
public:
  CHexaMeshRegionEntry_Delegate(CHexaMeshRegionEntry* hexaMeshRegionEntry);

  virtual bool Attributes();

  virtual void AppendContextMenu(CContextMenuInvoker& invoker);

  bool CanCreateMeshRegion() const;

  void CreateMeshRegionPoly();
  void CreateMeshRegionRect();

private:
  CHexaMeshRegionEntry_Delegate(const CHexaMeshRegionEntry_Delegate& rhs);
  CHexaMeshRegionEntry_Delegate& operator = (
    const CHexaMeshRegionEntry_Delegate& rhs);

  CHexaMeshRegionEntry* m_hexaMeshRegionEntry;

  REGISTER_DELEGATE(CHexaMeshRegionEntry, CHexaMeshRegionEntry_Delegate);
};

#endif  // _HexaMeshRegion_Delegate_h_
