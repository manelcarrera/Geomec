// DCLoadManager.h: interface for the CLoadManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DCLOADMANAGER_H__D27A86C2_599B_421A_BC7F_DAD45262CFB3__INCLUDED_)
#define AFX_DCLOADMANAGER_H__D27A86C2_599B_421A_BC7F_DAD45262CFB3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "IMesh.h"

namespace dia {

class ILoad;
class ICase;
class CLoadCase;
class CBoundaCase;
class CLoadCombination;
class IDianaRunner;

#include "DCInitialNodalPotential.h"

class CLoadManager {
  friend class ILoad;
  friend class dia::CInitialNodalPotential;

  const geo::IMesh &m_Mesh;
  IDianaRunner &m_Runner;

  std::vector<CLoadCase *> m_vcLoadCases;
  std::vector<CBoundaCase *> m_vcBoundaCases;
  std::vector<CLoadCombination *> m_vcLoadCombinations;
  std::set<ILoad *> m_stLoads;
  std::vector<dia::CInitialNodalPotential *> m_vcInitialNodalPotentials;

  // called by constructor of (friend) ILoad
  bool InsertLoad(ILoad &load);

  // called by constructor of (friend) CInitialNodalPotential, returns the index
  int InsertInitialNodalPotential(dia::CInitialNodalPotential &potential);

public:
  CLoadManager(const geo::IMesh &mesh, IDianaRunner &runner);
  virtual ~CLoadManager();

  void Clear();

  int LoadCaseSize() const;
  const CLoadCase &LoadCase(int nCase) const;
  CLoadCase &LoadCase(int nCase);

  CLoadCase &NewLoadCase();

  int BoundaCaseSize() const;
  const CBoundaCase &BoundaCase(int nCase) const;
  CBoundaCase &BoundaCase(int nCase);

  CBoundaCase &NewBoundaCase();

  int CaseSize() const;
  const ICase &Case(int nCase) const;
  ICase &Case(int nCase);

  int LoadCombinationSize() const;
  const CLoadCombination &LoadCombination(int nCombination) const;
  CLoadCombination &LoadCombination(int nCombination);

  CLoadCombination &NewLoadCombination();

  bool WriteFilos() const;

  const geo::IMesh &Mesh() const;

  const IDianaRunner &Runner() const;
  IDianaRunner &Runner();
};

} // namespace dia

#endif // !defined(AFX_DCLOADMANAGER_H__D27A86C2_599B_421A_BC7F_DAD45262CFB3__INCLUDED_)
