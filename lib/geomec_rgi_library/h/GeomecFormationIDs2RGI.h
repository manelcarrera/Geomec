#ifndef _GeomecFormationIDs2RGI_h_
#define _GeomecFormationIDs2RGI_h_

class RGInterface;
class CModelBase;
class CGraphNode;
class CFormationBase;

#include <set>
#include <vector>

#include "RGIFormationIDs.h"

namespace GeomecRGI {

class CGeomecFormationIDs2RGI {
public:
  CGeomecFormationIDs2RGI(RGInterface &rgi, const CModelBase &modelBase);
  ~CGeomecFormationIDs2RGI();

  std::vector<int> getFormationIDs() const;
  CFormationBase *getFormation(int rgiFormationID) const;

  static void SetFormations(RGInterface &rgi, const CModelBase &modelBase);

private:
  CGeomecFormationIDs2RGI(const CGeomecFormationIDs2RGI &rhs);
  CGeomecFormationIDs2RGI &operator=(const CGeomecFormationIDs2RGI &rhs);

  const CModelBase &m_modelBase;
  CRGIFormationIDs m_rgiFormationIDs;
  std::vector<std::vector<std::vector<double>>> m_elements;
  std::vector<CFormationBase *> m_formations;
  std::vector<int> m_GeomecFormationIDs;
  std::set<int> m_distinctGeomecFormationIDs;
  std::map<int, CFormationBase *> m_rgiFormationID2GeomecFormation;
};

} // namespace GeomecRGI

#endif // _GeomecFormationIDs2RGI_h_
