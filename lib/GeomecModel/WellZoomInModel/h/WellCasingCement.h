#ifndef _WELLCASINGCEMENT_H_
#define _WELLCASINGCEMENT_H_

#include "FormationBase.h"
#include "WellCasingMesh.h"

class CWellCasingModel;
class CWellCasingCementInitialMaterial;

class CWellCasingCement : public CFormationBase
{
  typedef CFormationBase TBase;

public:
  CWellCasingCement(const QString& strName, CWellCasingModel& model);
  CWellCasingCement(CWellCasingModel& model);
  ~CWellCasingCement();

  virtual unsigned int IconId() const;
  virtual unsigned int TypeId() const;
  virtual QString TypeName() const;
  virtual bool Less(const CGraphNode& node) const;

  virtual void OnNeighbourModified(const CGraphNode& node, enum ModifiedHint uHint);

  virtual bool Empty() const;

  virtual void LoadStream(TSTREAM& stream, CStreamVersion& version, TPROGRESS& progress);

  virtual int DisplayListSize() const;
  virtual const geo::IObject& DisplayList(int nIndex) const;

  const CWellCasingCementInitialMaterial& GetCementMaterial(const CFFMaterial& base) const;

  void OnShowElements();
  void OnShowMidpoints();

  ACCEPT_GEOMECMODELVISITORS(VisitWellCasingCement);

private:

  void InvalidateCache();

  class CMaterialLess
  {
  public:
  bool operator()(const CWellCasingCementInitialMaterial* m1, const CWellCasingCementInitialMaterial* m2) const;
  };

  typedef std::set<CWellCasingCementInitialMaterial*, CMaterialLess> TMaterialSet;
  mutable TMaterialSet m_stMaterials;

  mutable CElementCenterPointCache m_centerpoints;
  bool m_bShowMidpoints;
};

#endif // _WELLCASINGCEMENT_H_
