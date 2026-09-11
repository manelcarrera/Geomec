// HexaFormation.h: interface for the CHexaFormation class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HEXAFORMATION_H__6A6ADA62_6169_414C_859D_AFC61D33450D__INCLUDED_)
#define AFX_HEXAFORMATION_H__6A6ADA62_6169_414C_859D_AFC61D33450D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CHexaHorizon;
class CFormationPlane;
class CHexaModel;

#include "3DFormation.h"
#include "FormationPlane.h"
#include "FormationVolume.h"
#include "GeomecModelVisitor.h"
#include "MeshSurface.h"

class CHexaFormationVolume : public CFormationVolume {
public:
  CHexaFormationVolume(C3DFormation &formation, geo::CBodyGroup &group);
  CHexaFormationVolume(C3DFormation &formation);
  virtual TGraphNodeSet Identifier() const;

  // Display lists (for skin mode)
  virtual int DisplayListSize() const;
  virtual const geo::IObject &DisplayList(int nIndex) const;

  virtual void OnNeighbourModified(const CGraphNode &node, enum ModifiedHint uHint);

  // actually an array of CMeshSurface pointers
  const geo::CPtrArray<geo::IObject> &Skin() const;

  ACCEPT_GEOMECMODELVISITORS(VisitHexaFormationVolume);

private:
  mutable CMeshSurface m_skinSides[4];
  mutable geo::CPtrArray<geo::IObject> m_skin;
  void buildSkin() const;
};

class CHexaFormation : public C3DFormation {
private:
  friend class CHexaHorizon;
  CHexaHorizon *m_pUpper;
  CHexaHorizon *m_pLower;
  int m_nElements;
  QSharedPointer<CFormationPlane> m_pFormationPlane[3];

private:
  virtual bool ShowCenterPoints() const;

public:
  int Elements() const;
  void Elements(int nElements);

  //	void Clear();

  bool CanSplitFormation(const CHexaHorizon &horizon) const;
  void SplitFormation(CHexaHorizon &horizon);
  // Construction / Destruction ....
  CHexaFormation(const CHexaFormation &rhs);
  CHexaFormation(CFemAppModel &model);
  CHexaFormation(const QString &strName, CHexaHorizon &upper, CHexaHorizon &lower, int nElements, CModelBase &model);
  virtual ~CHexaFormation();

  CHexaFormation &operator=(const CHexaFormation &rhs);
  bool operator==(const CHexaFormation &rhs) const;

  // Horizon access
  const CHexaHorizon &UpperHorizon() const;
  CHexaHorizon &UpperHorizon();
  const CHexaHorizon &LowerHorizon() const;
  CHexaHorizon &LowerHorizon();
  void UpperHorizon(CHexaHorizon &upper);
  void LowerHorizon(CHexaHorizon &lower);
  const CHexaFormation *UpperFormation() const;
  const CHexaFormation *LowerFormation() const;
  virtual bool Less(const CGraphNode &node) const;

  IValueDomainScalar::TValueVec CalculatePorePressure(const geo::IBody &body,
                                                      const CDepletionStage &pDepletionStage) const;
  //	std::vector<CTensor> CHexaFormation::CalculateEffectiveStress(const geo::CHexahedron &hexahedron) const;

  // save and load
  // Stream
  virtual void LoadStream(TSTREAM &stream, CStreamVersion &version, TPROGRESS &progress);
  virtual void SaveStream(TSTREAM &stream, TPROGRESS &progress);

  virtual long SavedItems() const;

  QSharedPointer<CFormationPlane> FormationPlane(CFormationPlane::ePlaneType type);
  const QSharedPointer<CFormationPlane> FormationPlane(CFormationPlane::ePlaneType type) const;

  ACCEPT_GEOMECMODELVISITORS(VisitHexaFormation);
};

// Visualisation ...
class CHexaFormationEntry : public CFormationEntryTempl<CHexaFormation> {
public:
  CHexaFormationEntry(CHexaModel &model);

  ACCEPT_GEOMECMODELVISITORS(VisitHexaFormationEntry);
};

#endif // !defined(AFX_HEXAFORMATION_H__6A6ADA62_6169_414C_859D_AFC61D33450D__INCLUDED_)
