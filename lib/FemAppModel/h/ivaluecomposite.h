// ivaluecomposite.h: interface for the IValueComposite class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IVALUECOMPOSITE_H__0C95A1E3_AB26_4555_9084_AB68C960A786__INCLUDED_)
#define AFX_IVALUECOMPOSITE_H__0C95A1E3_AB26_4555_9084_AB68C960A786__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "StorageNode.h"
#include "ivaluecomponent.h"

class IValueComposite : public CStorageNode {
  friend class IValueComponentBase;
  typedef std::vector<IValueComponentBase *> TComponentVec;
  typedef std::vector<TComponentVec> TModeVec;
  TModeVec m_vcMode;

public:
  typedef enum _enumWeightingType { NO_WEIGHTING = 0, VOLUME, INVERSE_VOLUME } TWeightingType;

  // Construction / Destruction
  IValueComposite(CFemAppModel &model);
  IValueComposite(const QString &strName, CFemAppModel &model);
  IValueComposite(unsigned int uName, CFemAppModel &model);
  IValueComposite(const IValueComposite &rhs);
  virtual ~IValueComposite();

  virtual QString ExportLabel() const { return Name(); }

  virtual bool IsTensorVector() const { return false; }

  // Mode interface ...
  virtual unsigned int ModeSize() const;                  // Returns number of modes > 0
  virtual QString ModeName(unsigned int uMode = 0) const; // Returns name of the mode

  // Component interface ...
  virtual unsigned int ComponentSize(unsigned int uMode = 0) const;
  virtual IValueComponentBase &Component(unsigned int uComponent = 0, unsigned int uMode = 0);
  virtual const IValueComponentBase &Component(unsigned int uComponent = 0, unsigned int uMode = 0) const;
  virtual void OnNeighbourModified(const CGraphNode &node, enum ModifiedHint uHint);
  virtual void OnNeighbourDeleted(const CGraphNode &node);

  // Construction / Destruction ...
  IValueComposite &operator=(const IValueComposite &rhs);
  bool operator==(const IValueComposite &rhs) const;

  // Save and load
  virtual bool Empty() const;

  virtual bool Less(const CGraphNode &rhs) const;
  virtual bool isFaultResult() const;
  virtual bool isMeshResult() const;
  virtual bool isMaterialResult() const;
  virtual bool isResult() const;

  virtual bool NeedParallelInitializationCallback() const;
  virtual geo::IParallelInitializationCallback *
  GetParallelInitializationCallback(); // factory method, caller owns the pointer
  virtual bool PrepareMapping(const geo::IElementSet *, const IValueComponentBase *);
  virtual void FinishMapping();

  virtual bool Accept(graphnode::IConstVisitor &visitor) const { return visitor.VisitIValueComposite(*this); }
  virtual bool Accept(graphnode::IVisitor &visitor) { return visitor.VisitIValueComposite(*this); }
};

typedef CGraphEntryTemp<IValueComposite> TValueCompositeEntry;

#endif // !defined(AFX_IVALUECOMPOSITE_H__0C95A1E3_AB26_4555_9084_AB68C960A786__INCLUDED_)
