#include "DCInitialNodalPotential.h"
#include "DCLoadManager.h"

#include <cassert>
#include "lbcx.h"
#include "lbfl.h"

namespace dia {

CInitialNodalPotential::CInitialNodalPotential(CLoadManager& loadmanager)
: m_nIndex(loadmanager.InsertInitialNodalPotential(*this))
{
}

CInitialNodalPotential::CInitialNodalPotential(CLoadManager& loadmanager, const TNodalPotentialMap& mpNodalValues)
: m_nIndex(loadmanager.InsertInitialNodalPotential(*this)),
  m_mpNodalValues(mpNodalValues)
{
}

void CInitialNodalPotential::Insert(int nNode, double dPotential)
{
  // set the value even if one already existed (overwrite)
  // use operator []
  m_mpNodalValues[nNode] = dPotential;
}

void CInitialNodalPotential::Remove(int nNode)
{
  TNodalPotentialMap::iterator it = m_mpNodalValues.find(nNode);
  if(it != m_mpNodalValues.end())
  m_mpNodalValues.erase(it);
}

int CInitialNodalPotential::Index() const
{
  return m_nIndex;
}

bool CInitialNodalPotential::WriteFilos() const
{
  PushDir();

  ChangeDir("/INIVAR");

  ftn_int_t idx = m_nIndex;
  assert(!XistIndexed("POTENT/", &idx));
  ChangeIndexedDir("POTENT/", &idx);

  int sz = m_mpNodalValues.size();
  ftn_int_t* pNodes = (ftn_int_t*)DiMalloc(sz * sizeof(ftn_int_t), "CInitialNodalPotential::WriteFilos");
  ftn_double_t* pValues = (ftn_double_t*)DiMalloc(sz * sizeof(ftn_double_t), "CInitialNodalPotential::WriteFilos");

  TNodalPotentialMap::const_iterator it;
  int i;
  for(it = m_mpNodalValues.begin(), i = 0; it != m_mpNodalValues.end(); ++it, ++i)
  {
  pNodes[i] = it->first;
  pValues[i] = it->second;
  }

  PutItemLength("NODES", pNodes, sz);
  PutItemLength("VALUES", pValues, sz);

  DiFree(pValues, "CInitialNodalPotential::WriteFilos");
  DiFree(pNodes, "CInitialNodalPotential::WriteFilos");

  PopDir();

  return true;
}

} // namespace dia
