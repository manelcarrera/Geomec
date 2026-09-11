#ifndef _cora_GetPointSetInfo_h_
#define _cora_GetPointSetInfo_h_

#include <fstream>

#include <QString>

class CModelBase;

#include "GetSetInfo.h"
#include "IPointSet.h"

namespace cora {

class CGetPointSetInfo : public CGetSetInfo {
public:
  CGetPointSetInfo(const CModelBase *modelBase);

  std::ostream &operator()(std::ostream &os) const;

private:
  CGetPointSetInfo(const CGetPointSetInfo &rhs);
  CGetPointSetInfo &operator=(CGetPointSetInfo rhs);

  void listPointSets(std::ostream &os, const QString &preFix, IPointSet::DIMENSION dimension) const;

  const CModelBase *m_modelBase;
};

} // namespace cora

std::ostream &operator<<(std::ostream &os, const cora::CGetPointSetInfo &i);

#endif // _cora_GetPointSetInfo_h_
