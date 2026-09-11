#if !defined(__QB_SETTINGS__)
#define __QB_SETTINGS__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GridDefinition.h"

class CModelBase;
class CFormationBase;
class CDepletionStage;

#include <map>

#include "Point.h"
#include "StorageNode.h"
#include "GeomecModelVisitor.h"

enum TTimeLapseType {Pressure, Temperature};

class CQBFormationIntersection
{
  const CFormationBase *m_pFormation;
  geo::CPoint m_TopIntersect;
  geo::CPoint m_BotIntersect;
public:
  CQBFormationIntersection(const CFormationBase *formation,
    const geo::CPoint &ptTop, const geo::CPoint &ptBot)
    : m_pFormation(formation), m_TopIntersect(ptTop), m_BotIntersect(ptBot){}

  const CFormationBase *Formation() { return m_pFormation; }
  const geo::CPoint &TopIntersect() { return m_TopIntersect; }
  const geo::CPoint &BotIntersect() { return m_BotIntersect; }
};
typedef std::vector<CQBFormationIntersection*> TFormationIntersects;
typedef std::map<int, TFormationIntersects> TGrid2Intersects;
class CQBSettings : public CStorageNode
{
  FILE *m_fpLog;

  TGrid2Intersects m_Intersections;

  CGridDefinition m_Grid;
  std::vector<std::pair<CDepletionStage*, CDepletionStage*> > m_DeltaPressures;
  std::vector<std::pair<CDepletionStage*, CDepletionStage*> > m_DeltaTemperatures;
  CModelBase &m_Model;
  std::set<const CFormationBase*> m_stSelectedFormations;

  CPointSet *m_QBPointSet;
public:
  CQBSettings(CModelBase &model);
  ~CQBSettings();

  void StartQB();
  void WriteDat(const QString &path);
  CGridDefinition &Grid() { return m_Grid; }
  const CGridDefinition &Grid() const { return m_Grid; }
  std::set<const CFormationBase*> &SelectedFormations() { return m_stSelectedFormations; }

  CModelBase &Model() { return m_Model; }
  const CModelBase &Model() const { return m_Model; }

  QString ListObjectName() const;

  void AddDeltaPressure(CDepletionStage *stage1, CDepletionStage *stage2);
  void RemoveDeltaPressure(int nIndex);
  const std::vector<std::pair<CDepletionStage*, CDepletionStage*> > &DeltaPressures();
  void AddDeltaTemperature(CDepletionStage *stage1, CDepletionStage *stage2);
  void RemoveDeltaTemperature(int nIndex);
  const std::vector<std::pair<CDepletionStage*, CDepletionStage*> > &DeltaTemperatures();
  void SetTimeLapse(int nIndex, CDepletionStage *time1, CDepletionStage *time2, TTimeLapseType TLT);

  CPointSet *PointSet() { return m_QBPointSet; }

  const TGrid2Intersects &Intersections() { return m_Intersections; }

  virtual void OnNeighbourModified(const CGraphNode& node, enum ModifiedHint uHint);
  virtual void OnNeighbourDeleted(const CGraphNode& node);

  virtual void LoadStream(TSTREAM& stream, CStreamVersion& version, TPROGRESS& progress);
	virtual void SaveStream(TSTREAM& stream, TPROGRESS& progress);
  virtual long SavedItems() const { return 0; }
  virtual unsigned int TypeId() const { return 0; }
  virtual unsigned int IconId() const { return 0; }
  virtual bool Empty() const { return false; }
  int IndexOffset() const { return 1; }

  ACCEPT_GEOMECMODELVISITORS(VisitQBSettings);

private:
  void GeneratePointSet(IProgressBase &progress);
  void AddDepths(IProgressBase &progress);
  QString QuickBlocksLogPath();

};

class CQBInputWriter
{
  CQBSettings &m_QBSettings;
  FILE *m_fp;
  QString m_strError;
public:
  CQBInputWriter(CQBSettings &settings);
  ~CQBInputWriter(void);

  bool WriteInput(const QString &sPath, IProgressBase &progress);
  const QString &Error() const { return m_strError; }
  int StepSize() const;
private:
  void PrintDepthsAndPressures(int index);
  void PrintHeader();
};
#endif
