#include "DCFixedPotential.h"

#include "INode.h"
#include "IProgressBase.h"

#include "lbfl.h"
#include "lbcx.h"

namespace dia {

CFixedPotential::CFixedPotential()
{
}

bool CFixedPotential::InsertNode(const geo::INode& node)
{
  return m_stNodeIndices.insert(node.Index()).second;
}

bool CFixedPotential::RemoveNode(const geo::INode& node)
{
  return (m_stNodeIndices.erase(node.Index()) != 0);
}

int CFixedPotential::NodeSize() const
{
  return m_stNodeIndices.size();
}

static void WriteFixPot(ftn_int_t nodeidx)
{
  ftn_int_t idx = Inquire("/FIXPOT", "DIM");
  if(idx < 0)
  idx = 0;
  ++idx;

  PushDir();
  ChangeIndexedDir("/FIXPOT/", &idx);
  PutItem("NODES", &nodeidx);
  PopDir();

  idx = Inquire("/INPUT/FIXPOT", "DIM");
  if(idx < 0)
  idx = 0;
  ++idx;

  PushDir();
  ChangeIndexedDir("/INPUT/FIXPOT/", &idx);
  PutCharItem("NAME", "fixpot");
  PutItemLength("NODES", &nodeidx, 1);
  PopDir();
}

bool CFixedPotential::WriteFilos(IProgressBase& progress) const
{
  TNodeIndexSet::const_iterator it;
  for(it = m_stNodeIndices.begin(); it != m_stNodeIndices.end(); ++it)
  {
  WriteFixPot(*it + 1);
  progress.Step();
  }

  return true;
}

}
