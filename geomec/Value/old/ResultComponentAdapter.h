// ResultComponentAdapter.h: interface for the CResultComponentAdapter class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RESULTCOMPONENTADAPTER_H__102ACF24_DE45_46A3_A53A_4563493BA959__INCLUDED_)
#define AFX_RESULTCOMPONENTADAPTER_H__102ACF24_DE45_46A3_A53A_4563493BA959__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class IResultComponent;
class CResultAdapter;
class CResultComponentAdapter : public IValueComponent {
  IResultComponent &m_component;

public:
  CResultComponentAdapter(CResultAdapter &result_adapter, IResultComponent &result_component);
  virtual ~CResultComponentAdapter();

  const IResultComponent &ResultComponent() const;
  IResultComponent &ResultComponent();

  // Min / Max
  virtual BOOL Defined() const;
  virtual TMinMax MinMax(const CQuantity::UNIT unit = CQuantity::SI_UNIT) const;
  virtual CString UnitName(const CQuantity::UNIT unit = CQuantity::SI_UNIT) const;
  virtual geo::CValue ValuePointAt(const geo::IPoint &pt, const CQuantity::UNIT unit = CQuantity::SI_UNIT) const;
  virtual TValueVec ValueElementAt(const geo::IElement &elm, const CQuantity::UNIT unit = CQuantity::SI_UNIT) const;
  virtual UINT IconId() const;
  virtual UINT TypeNameId() const;
  virtual CString ExportLabel() const;

  // Cannot saved ...
  virtual BOOL Empty() const;
  virtual void Load(TLOCATION &location, TPROGRESS &progress);
  virtual void Save(TLOCATION &location, TPROGRESS &progress);
  virtual long SavedItems() const;
};

#endif // !defined(AFX_RESULTCOMPONENTADAPTER_H__102ACF24_DE45_46A3_A53A_4563493BA959__INCLUDED_)
