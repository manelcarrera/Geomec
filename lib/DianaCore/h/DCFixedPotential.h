#ifndef _DCFIXEDPOTENTIAL_H_
#define _DCFIXEDPOTENTIAL_H_

#include <set>

class IProgressBase;

namespace geo {
class INode;
}

namespace dia {

class IDianaRunner;

class CFixedPotential {
  typedef std::set<int> TNodeIndexSet;
  TNodeIndexSet m_stNodeIndices;

  friend class IDianaRunner;

  CFixedPotential();

public:
  bool InsertNode(const geo::INode &node);
  bool RemoveNode(const geo::INode &node);

  int NodeSize() const;

  bool WriteFilos(IProgressBase &progress) const;
};

} // namespace dia

#endif // _DCFIXEDPOTENTIAL_H_
