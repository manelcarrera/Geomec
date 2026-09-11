#ifndef _DCINITIALNODALPOTENTIAL_H_
#define _DCINITIALNODALPOTENTIAL_H_

#include <map>

namespace dia {

class CLoadManager;

// instances of this class must be created on the heap (with new)
// ownership is transferred to the provided loadmanager
class CInitialNodalPotential {
public:
  typedef std::map<int, double> TNodalPotentialMap; // maps from global node index to potential value

public:
  CInitialNodalPotential(CLoadManager &loadmanager);
  CInitialNodalPotential(CLoadManager &loadmanager, const TNodalPotentialMap &mpNodalValues);

  void Insert(int nNode, double dPotential);
  void Remove(int nNode);

  int Index() const;

  bool WriteFilos() const;

private:
  int m_nIndex;
  TNodalPotentialMap m_mpNodalValues;
};

} // namespace dia

#endif // _DCINITIALNODALPOTENTIAL_H_
