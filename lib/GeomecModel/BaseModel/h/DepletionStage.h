// DepletionStage.h: interface for the CDepletionStage class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DEPLETIONSTAGE_H__C54A592D_5885_4A43_BF13_F3295EFAC363__INCLUDED_)
#define AFX_DEPLETIONSTAGE_H__C54A592D_5885_4A43_BF13_F3295EFAC363__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GeomecTime.h"
class CDepletionStageEntry;

#include "GeomecModelVisitor.h"
#include "StorageNode.h"

class CDepletionStage : public CStorageNode {
public:
  enum eConvergenceType { FORCE, DISPLACEMENT, ENERGY };                 // Is saved ...
  enum eIterationScheme { CONSTANT, LINEAR, REGULAR, MODIFIED, SECANT }; // Is saved ...
  enum eOutputType { NONE, RESULTS, BRANCH, PHASE };                     // Is saved ...

  static const char *m_output_type_cp[];

private:
  CDepletionStage *m_pPrevious;
  CDepletionStage *m_pNext;
  CGeomecTime m_Time;
  CDepletionStageEntry &m_entry;

  // marked as initial stage (zero strains and displacements)
  bool m_bMarkedAsInitial;

  // Non-lin parameters
  int m_nSteps;
  double m_dStepScalingFactor;
  double m_dConvNormVal;
  int m_nIterationSteps;
  eConvergenceType m_convergence_type;
  eIterationScheme m_iteration_scheme;
  eOutputType m_output_type;
  bool m_bAutoStepSizes;
  double m_dMaxAutoStepSize;

public:
  // Set non-line params
  void SetNonLinParams(bool bAutoStepSizes, double dMaxAutoStepSize, int nSteps, const double &dStepScalingFactor,
                       const double &dConvNormVal, int nIterationSteps, eConvergenceType convergence_type,
                       eIterationScheme iteration_scheme);
  void SetDefaultNonLinParams();
  int NLSteps() const;
  const double &NLStepScalingFactor() const;
  const double &NLConvNormVal() const;
  int NLIterationSteps() const;
  eConvergenceType NLConvergenceType() const;
  eIterationScheme NLIterationScheme() const;
  void setNLIterationScheme(eIterationScheme itScheme);
  bool AutomaticStepSizes() const;
  double MaxAutomaticStepSize() const;

  void setOutputType(eOutputType output_type);
  eOutputType OutputType() const;
  bool IsPhaseStartStage() const; // is the first of a new phase (initial or previous is phase/branch)

  const CGeomecTime &Time() const;
  void Time(const CGeomecTime &time);

  void MarkAsInitial();
  bool IsMarkedAsInitial() const;
  bool CanMarkAsInitial() const;

  // ##ModelId=3BC55D630287
  virtual unsigned int TypeId() const;
  CDepletionStage(CDepletionStageEntry &entry);
  // ##ModelId=3BC55D630297
  CDepletionStage(const QString &strName, CDepletionStageEntry &entry, int nSteps = 10,
                  const double &dStepScalingFactor = 1., const double &dConvNormVal = 0.001, int nIterationSteps = 5,
                  eConvergenceType convergence_type = FORCE, eIterationScheme iteration_scheme = REGULAR,
                  eOutputType output_type = DefaultOutputType(), bool bAutoStepSizes = false,
                  double dMaxAutoStepSize = 0.3);
  // ##ModelId=3BC55D63029A
  CDepletionStage(const QString &strName, CDepletionStage &previous, int nSteps = 10,
                  const double &dStepScalingFactor = 1., const double &dConvNormVal = 0.001, int nIterationSteps = 5,
                  eConvergenceType convergence_type = FORCE, eIterationScheme iteration_scheme = REGULAR,
                  eOutputType output_type = DefaultOutputType(), bool bAutoStepSizes = false,
                  double dMaxAutoStepSize = 0.3);
  CDepletionStage(const QString &strName, CDepletionStage *previous, CDepletionStage *next, const CGeomecTime &time,
                  int nSteps = 10, const double &dStepScalingFactor = 1., const double &dConvNormVal = 0.001,
                  int nIterationSteps = 5, eConvergenceType convergence_type = FORCE,
                  eIterationScheme iteration_scheme = REGULAR, eOutputType output_type = DefaultOutputType(),
                  bool bAutoStepSizes = false, double dMaxAutoStepSize = 0.3);

  CDepletionStage(const CDepletionStage &rhs);
  CDepletionStage &operator=(const CDepletionStage &rhs);
  bool operator==(const CDepletionStage &rhs) const;

  // ##ModelId=3BC55D6302A8
  virtual ~CDepletionStage();
  // ##ModelId=3BC55D6302B5
  virtual unsigned int IconId() const;
  // ##ModelId=3BC55D6302B7
  const CDepletionStage &Previous() const;
  // ##ModelId=3BC55D6302B9
  CDepletionStage &Previous();
  // ##ModelId=3BC55D6302BA
  const CDepletionStage &Next() const;
  // ##ModelId=3BC55D6302BC
  CDepletionStage &Next();
  // ##ModelId=3BC55D6302C5
  bool Last() const;
  // ##ModelId=3BC55D6302C7
  bool Initial() const;

  const CDepletionStage &InitialStage() const;
  CDepletionStage &InitialStage();
  const CDepletionStage &LastStage() const;
  CDepletionStage &LastStage();

  const CDepletionStageEntry &Entry() const;
  CDepletionStageEntry &Entry();

  using CStorageNode::Index; // would otherwise be hidden by function below
  int Index() const;         // initial = 0, etc.

  // ##ModelId=3BE7AEC5013D
  virtual bool Less(const CGraphNode &node) const;
  // Save and load
  virtual bool Empty() const;
  virtual long SavedItems() const;

  virtual void LoadStream(TSTREAM &stream, CStreamVersion &version, TPROGRESS &progress);
  virtual void SaveStream(TSTREAM &stream, TPROGRESS &progress);

  bool isAfter(const CDepletionStage &other) const;
  bool isBefore(const CDepletionStage &other) const;

  const CDepletionStage &GetNextBranchStage(const CDepletionStage &stage) const;
  std::string ModifiedFileName(const std::string &fileName) const;

  static std::string IterationSchemeTag(eIterationScheme scheme);
  static std::string ConvergenceTypeTag(eConvergenceType type);
  static std::string OutputTypeTag(eOutputType type);

  static eOutputType DefaultOutputType();

  const CDepletionStage *Next_();
  const CDepletionStage *Previous_();

  ACCEPT_GEOMECMODELVISITORS(VisitDepletionStage);
};

class CConstDepletionStageIterator {
public:
  CConstDepletionStageIterator();
  CConstDepletionStageIterator(const CDepletionStage &stage);
  CConstDepletionStageIterator(const CConstDepletionStageIterator &rhs);
  CConstDepletionStageIterator &operator=(const CConstDepletionStageIterator &rhs);

  bool operator==(const CConstDepletionStageIterator &rhs) const;
  bool operator!=(const CConstDepletionStageIterator &rhs) const;

  CConstDepletionStageIterator &operator++();
  CConstDepletionStageIterator operator++(int);
  CConstDepletionStageIterator &operator--();
  CConstDepletionStageIterator operator--(int);

  const CDepletionStage &operator*();
  const CDepletionStage *operator->();

private:
  const CDepletionStage *m_pStage;
};

class CDepletionStageIterator : public CConstDepletionStageIterator {
public:
  CDepletionStageIterator();
  CDepletionStageIterator(CDepletionStage &stage);
  CDepletionStageIterator(const CDepletionStageIterator &rhs);
  CDepletionStageIterator &operator=(const CDepletionStageIterator &rhs);

  CDepletionStageIterator &operator++();
  CDepletionStageIterator operator++(int);
  CDepletionStageIterator &operator--();
  CDepletionStageIterator operator--(int);

  CDepletionStage &operator*();
  CDepletionStage *operator->();
};

typedef CGraphEntryTemp<CDepletionStage> TDepletionStageEntry;
class CDepletionStageEntry : public CGraphEntryTemp<CDepletionStage> {
  friend class CDepletionStage;
  CDepletionStage *m_pFirstStage;

public:
  /*
  enum
  {
    MarkedAsInitialChanged = 1
  };
  */

  typedef CConstDepletionStageIterator const_iterator;
  typedef CDepletionStageIterator iterator;

  CDepletionStageEntry(unsigned int uEntryID, CFemAppModel &model);
  virtual ~CDepletionStageEntry();

  void LoadStream(CStorageNode::TSTREAM &stream, CStreamVersion &version, CStorageNode::TPROGRESS &progress);
  void SaveStream(CStorageNode::TSTREAM &stream, CStorageNode::TPROGRESS &progress) const;
  long SavedItems() const;

  const CDepletionStage &InitialStage() const;
  CDepletionStage &InitialStage();

  const CDepletionStage &LastStage() const;
  CDepletionStage &LastStage();

  const CDepletionStage &MarkedAsInitialStage() const;
  CDepletionStage &MarkedAsInitialStage();

  const CDepletionStage &StageByIndex(int nIndex) const;
  CDepletionStage &StageByIndex(int nIndex);

  const_iterator begin() const;
  const_iterator end() const;

  iterator begin();
  iterator end();

  int StageIndexFromName(QString strName) const;

  ACCEPT_GEOMECMODELVISITORS(VisitDepletionStageEntry);
};

#endif // !defined(AFX_DEPLETIONSTAGE_H__C54A592D_5885_4A43_BF13_F3295EFAC363__INCLUDED_)
