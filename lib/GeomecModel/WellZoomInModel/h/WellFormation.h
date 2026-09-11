#ifndef _WELLZOOMINFORMATION_H_
#define _WELLZOOMINFORMATION_H_

#include "3DFormation.h"
#include "GeomecModelVisitor.h"

class IWellModel;

class CWellFormation : public C3DFormation
{
public:
  CWellFormation(const CFormationBase& parent, unsigned int nEntryType, IWellModel& model);
  CWellFormation(CFemAppModel& model);

  virtual bool ShowCenterPoints() const;
  virtual bool Less(const CGraphNode &node) const;

  virtual void LoadStream(TSTREAM& stream, CStreamVersion& version, TPROGRESS& progress);
  virtual void SaveStream(TSTREAM& stream, TPROGRESS& progress);
  virtual long SavedItems() const;

  ACCEPT_GEOMECMODELVISITORS(VisitWellFormation);

protected:
  virtual void FindParentFormation();

  virtual void LoadMaterial(CDepletionStage& stage, TSTREAM& stream, CStreamVersion& version, TPROGRESS& progress);

private:
  void DuplicateParentMaterials();
  void DuplicateLoads();
};

class CWellFormationEntry : public CFormationEntryTempl<CWellFormation>
{
public:
  CWellFormationEntry(int nEntryId, IWellModel& model);

  IMaterialRock& CloneMaterial(const IMaterialRock& parentmat);

  ACCEPT_GEOMECMODELVISITORS(VisitWellFormationEntry);

private:
  typedef std::map<const IMaterialRock*, IMaterialRock*> TMaterialMap; // maps from parent materials to own copies
  TMaterialMap m_mpMaterials;
};

#endif // _WELLZOOMINFORMATION_H_
