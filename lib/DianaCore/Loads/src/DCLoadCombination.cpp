// DCLoadCombination.cpp: implementation of the CLoadCombination class.
//
//////////////////////////////////////////////////////////////////////

#include "DCLoadManager.h"
#include "DCLoadCombination.h"

#include "DCLoadCase.h"

#include "lbcx.h"
#include "lbfl.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

namespace dia {

CLoadCombination::CLoadCombination(CLoadManager &manager, int index)
:	m_Manager(manager), m_Index(index)
{
}

CLoadCombination::~CLoadCombination()
{
}

int CLoadCombination::Index() const
{
	return m_Index;
}

CLoadManager &CLoadCombination::Manager()
{
	return m_Manager;
}

const CLoadManager &CLoadCombination::Manager() const
{
	return m_Manager;
}

const double &CLoadCombination::Factor(const CLoadCase &LoadCase) const
{
	TFactorMap::const_iterator it = m_Factors.find(&LoadCase);
	assert(it != m_Factors.end());

	return it->second;
}

void CLoadCombination::InsertLoadCase(const CLoadCase &LoadCase, double dFactor /* = 1.0 */)
{
	m_Factors.insert(std::make_pair(&LoadCase, dFactor));
}

bool CLoadCombination::WriteFilos() const
{
	ftn_int_t idx = (ftn_int_t) (Index() + 1);
	assert(!XistIndexed("/COMBIN/", &idx));

	PushDir();

	ChangeIndexedDir("/COMBIN/", &idx);

	ftn_int_t *pCases = (ftn_int_t *) DiMalloc(m_Factors.size() * sizeof (ftn_int_t), "CLoadCombination::WriteFilos");
	ftn_double_t *pFactors = (ftn_double_t *) DiMalloc(m_Factors.size() * sizeof (ftn_double_t), "CLoadCombination::WriteFilos");

	int i = 0;
	for(TFactorMap::const_iterator it = m_Factors.begin(); it != m_Factors.end(); it++)
	{
		pCases[i] = (ftn_int_t) (it->first->Index() + 1);
		pFactors[i] = (ftn_double_t) it->second;
		
		i++;
	}

	PutItemLength("CASES", pCases, m_Factors.size());
	PutItemLength("FACTOR", pFactors, m_Factors.size());

	DiFree(pFactors, "CLoadCombination::WriteFilos");
	DiFree(pCases, "CLoadCombination::WriteFilos");

	PopDir();

	return true;
}

} // namespace dia
