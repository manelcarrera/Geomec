// ValueType.h: interface for the CValueType class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VALUETYPE_H__2985019B_007B_4A97_88EB_B7B529265A06__INCLUDED_)
#define AFX_VALUETYPE_H__2985019B_007B_4A97_88EB_B7B529265A06__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class IPointSet;

#include "GeomecModelVisitor.h"
#include "GeomecStringTable.h"
#include "ivaluecomposite.h"

class CValueType : public IValueComposite {
  IPointSet *m_pPointSet;

public:
  enum TMapEnum {
    MT_NONE,   // no weighting
    MT_VOIGHT, // volume weighted
    MT_REUSS,  // inverse volume weighted
    MT_POINT   // smoothed point interpolation
  };

private:
  enum TMapEnum m_map_type;

public:
  // Construction / Destruction
  CValueType(IPointSet &point_set, const QString &strName);
  CValueType(IPointSet &point_set, unsigned int uName);
  CValueType(const CValueType &rhs);
  virtual ~CValueType();

  virtual bool SingleElementValue() const;

  // Notify
  virtual void OnNeighbourDeleted(const CGraphNode &node);

  // Copy functions
  virtual unsigned int IconId() const;
  virtual unsigned int TypeId() const;
  virtual QString TypeName() const;

  // Point set access
  const IPointSet &PointSet() const;
  IPointSet &PointSet();

  // Weigthing
  enum TMapEnum MapType() const;
  void MapType(enum TMapEnum type);

  // Assignment
  CValueType &operator=(const CValueType &rhs);
  bool operator==(const CValueType &rhs) const;

  // Export
  virtual bool CanExport() const;

  // Destroying
  virtual bool Destroy();
  virtual bool CanDestroy() const;

  virtual QString ExportType() const;

  // Save and load
  virtual bool Empty() const;
  virtual void LoadStream(TSTREAM &stream, CStreamVersion &version, TPROGRESS &progress);
  virtual void SaveStream(TSTREAM &stream, TPROGRESS &progress);
  virtual long SavedItems() const;
  virtual void OnNeighbourModified(const CGraphNode &node, enum ModifiedHint uHint);

  virtual bool PrepareMapping(const geo::IElementSet *pElementSet);

  ACCEPT_GEOMECMODELVISITORS(VisitValueType);
};

template <unsigned int uTypeId,      // ID of the type exported
          unsigned int uIconId,      // ID of the Icon
          unsigned int uValueNameId, // ID of the name
          unsigned int uExportId>    // ID of the exported type
class CValueTypeTemp : public CValueType {
public:
  CValueTypeTemp(IPointSet &point_set, const QString &strName) : CValueType(point_set, strName) {}

  CValueTypeTemp(const CValueTypeTemp &rhs) : CValueType(rhs) {}

  virtual QString TypeName() const { return getStringTableEntry(uValueNameId); }

  virtual QString ExportType() const { return getStringTableEntry(uExportId); }

  virtual unsigned int TypeId() const { return uTypeId; }

  virtual unsigned int IconId() const { return uIconId; }

  static unsigned int type_id() { return uTypeId; }
  static unsigned int icon_id() { return uIconId; }
  static unsigned int name_id() { return uValueNameId; }
  static unsigned int export_id() { return uExportId; }

  virtual void OnNeighbourModified(const CGraphNode &node, enum ModifiedHint uHint) {
    CValueType::OnNeighbourModified(node, uHint);
  }
};

#endif // !defined(AFX_VALUETYPE_H__2985019B_007B_4A97_88EB_B7B529265A06__INCLUDED_)
