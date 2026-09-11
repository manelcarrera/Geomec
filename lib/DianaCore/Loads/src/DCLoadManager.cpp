// DCLoadManager.cpp: implementation of the CLoadManager class.
//
//////////////////////////////////////////////////////////////////////

#include "DCLoadCase.h"
#include "DCBoundaCase.h"
#include "DCLoadCombination.h"
#include "DCLoadManager.h"
#include "IDCLoad.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

namespace dia {

CLoadManager::CLoadManager(const geo::IMesh &mesh, IDianaRunner &runner)
:	m_Mesh(mesh), m_Runner(runner)
{
}

CLoadManager::~CLoadManager()
{
	Clear();
}

void CLoadManager::Clear()
{
	for(std::set<ILoad *>::iterator it = m_stLoads.begin(); it != m_stLoads.end(); it++) delete *it;
	m_stLoads.clear();

  for(size_t i = 0; i < m_vcInitialNodalPotentials.size(); ++i)
    delete m_vcInitialNodalPotentials[i];

  m_vcInitialNodalPotentials.clear();
}

int CLoadManager::LoadCaseSize() const
{
	return m_vcLoadCases.size();
}

const CLoadCase &CLoadManager::LoadCase(int nCase) const
{
	return *m_vcLoadCases[nCase];
}

CLoadCase &CLoadManager::LoadCase(int nCase)
{
	return *m_vcLoadCases[nCase];
}

CLoadCase &CLoadManager::NewLoadCase()
{
	CLoadCase *pNew = new CLoadCase(*this, LoadCaseSize());
	m_vcLoadCases.push_back(pNew);

	return *pNew;
}

int CLoadManager::BoundaCaseSize() const
{
  return m_vcBoundaCases.size();
}

const CBoundaCase &CLoadManager::BoundaCase(int nCase) const
{
  return *m_vcBoundaCases[nCase];
}

CBoundaCase &CLoadManager::BoundaCase(int nCase)
{
  return *m_vcBoundaCases[nCase];
}

CBoundaCase &CLoadManager::NewBoundaCase()
{
  CBoundaCase* pNew = new CBoundaCase(*this, BoundaCaseSize());
  m_vcBoundaCases.push_back(pNew);

  return *pNew;
}

int CLoadManager::CaseSize() const
{
  return LoadCaseSize() + BoundaCaseSize();
}

const ICase& CLoadManager::Case(int nCase) const
{
  if(nCase < LoadCaseSize())
    return LoadCase(nCase);

  return BoundaCase(nCase - LoadCaseSize());
}

ICase& CLoadManager::Case(int nCase)
{
  if(nCase < LoadCaseSize())
    return LoadCase(nCase);

  return BoundaCase(nCase - LoadCaseSize());
}

int CLoadManager::LoadCombinationSize() const
{
	return m_vcLoadCombinations.size();
}

const CLoadCombination &CLoadManager::LoadCombination(int nCombination) const
{
	return *m_vcLoadCombinations[nCombination];
}

CLoadCombination &CLoadManager::LoadCombination(int nCombination)
{
	return *m_vcLoadCombinations[nCombination];
}

CLoadCombination &CLoadManager::NewLoadCombination()
{
	CLoadCombination *pNew = new CLoadCombination(*this, LoadCombinationSize());
	m_vcLoadCombinations.push_back(pNew);

	return *pNew;
}

bool CLoadManager::InsertLoad(ILoad &load)
{
	return m_stLoads.insert(&load).second;
}

// called by constructor of (friend) CInitialNodalPotential, returns the index
int CLoadManager::InsertInitialNodalPotential(dia::CInitialNodalPotential& potential)
{
  int idx = m_vcInitialNodalPotentials.size();
  m_vcInitialNodalPotentials.push_back(&potential);
  return idx;
}

bool CLoadManager::WriteFilos() const
{
	int i;

	for(i = 0; i < CaseSize(); i++)
		if(!Case(i).WriteFilos()) return false;

	for(i = 0; i < LoadCombinationSize(); i++)
		if(!LoadCombination(i).WriteFilos()) return false;

	return true;
}

const geo::IMesh &CLoadManager::Mesh() const
{
	return m_Mesh;
}

const IDianaRunner &CLoadManager::Runner() const
{
	return m_Runner;
}

IDianaRunner &CLoadManager::Runner()
{
	return m_Runner;
}

} // namespace dia
