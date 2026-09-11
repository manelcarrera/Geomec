#ifndef _WELLCASINGNODE_H_
#define _WELLCASINGNODE_H_

class CWellCasingModel;
class CWellCasingSteel;
class CWellCasingCementInterface;
class CWellCasingInternalPressure;
class CWellCasingInternalTemperature;
class CDepletionStage;

#include "openglnode.h"
#include "GeomecModelVisitor.h"

class CWellCasingNode : public COpenGLNode
{
public:
  CWellCasingNode(CWellCasingModel& model);

	virtual unsigned int IconId() const;
	virtual unsigned int TypeId() const;

	virtual bool Empty() const;

  virtual int DisplayListSize() const;
	virtual const geo::IObject& DisplayList(int nIndex) const;
	virtual TColor Color() const;
	virtual std::vector<CDrawDef::TColor> OnColor(const geo::IObject &object) const;

  virtual void OnNewNeighbour(const CGraphNode& node);
  virtual void OnNeighbourDeleted(const CGraphNode& node);
  virtual void OnNeighbourModified(const CGraphNode& node, enum ModifiedHint uHint);

  CWellCasingSteel& Steel();
  const CWellCasingSteel& Steel() const;

  CWellCasingCementInterface& CementInterface();
  const CWellCasingCementInterface& CementInterface() const;

  CWellCasingInternalPressure& InternalPressure(const CDepletionStage& stage);
  const CWellCasingInternalPressure& InternalPressure(const CDepletionStage& stage) const;

  CWellCasingInternalTemperature& InternalTemperature(const CDepletionStage& stage);
  const CWellCasingInternalTemperature& InternalTemperature(const CDepletionStage& stage) const;

  virtual void SaveStream(TSTREAM& stream, TPROGRESS& progress);
  virtual void LoadStream(TSTREAM& stream, CStreamVersion& version, TPROGRESS& progress);
  virtual long SavedItems() const;

  ACCEPT_GEOMECMODELVISITORS(VisitWellCasingNode);

private:
  void UpdatePressuresAndTemperatures();

private:
  CWellCasingSteel* m_pSteel;
  CWellCasingCementInterface* m_pCementInterface;
  typedef std::map<const CDepletionStage*, CWellCasingInternalPressure*> TPressureMap;
  TPressureMap m_mpPressures;
  typedef std::map<const CDepletionStage*, CWellCasingInternalTemperature*> TTemperatureMap;
  TTemperatureMap m_mpTemperatures;
};

#endif  // _WELLCASINGNODE_H_
