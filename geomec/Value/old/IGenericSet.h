// IGenericSet.h: interface for the IGenericSet class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IGENERICSET_H__7F95EA3F_12D6_46E1_A696_96AFC3733BCB__INCLUDED_)
#define AFX_IGENERICSET_H__7F95EA3F_12D6_46E1_A696_96AFC3733BCB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CValueSet;
class IGenericSet : public COpenGLNode {
  friend class CValueSet;
  std::vector<CValueSet *> m_vcValueSet;

public:
  IGenericSet(CGraphModel &model);
  IGenericSet(const CString &strName, CGraphModel &model);
  IGenericSet::IGenericSet(const CString &sName, CGraphModel &model, int nValueSetSize);
  IGenericSet::IGenericSet(const IGenericSet &rhs);
  virtual ~IGenericSet();

  IGenericSet &operator=(const IGenericSet &rhs);
  bool IGenericSet::operator==(const IGenericSet &rhs) const;

  // ValueSet
  int AddValueSet(CValueSet::eType type);
  int ValueSetSize() const;
  const CValueSet &ValueSet(int nIndex) const;
  CValueSet &ValueSet(int nIndex);
  int ValueSetIndex(const CValueSet &value_set) const;

  // Contents
  virtual int ElementSize() const = 0;
  virtual const geo::IElement &Element(int nIndex) const = 0;
  virtual int PointSize() const = 0;
  virtual const geo::IPoint &PointAt(int nIndex) const = 0;

  // OK
  virtual void OnNeighbourDeleted(const CGraphNode &item);

  // Save and load
  virtual void Load(TLOCATION &location, TPROGRESS &progress);
  virtual void Save(TLOCATION &location, TPROGRESS &progress);
  virtual long SavedItems() const;
};

#endif // !defined(AFX_IGENERICSET_H__7F95EA3F_12D6_46E1_A696_96AFC3733BCB__INCLUDED_)
