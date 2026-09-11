#ifndef _cora_GetElementSetInfo_h_
#define _cora_GetElementSetInfo_h_

#include <fstream>

class CModelBase;

#include "GetSetInfo.h"

namespace cora {

class CGetElementSetInfo : public CGetSetInfo {
public:
  CGetElementSetInfo(const CModelBase *modelBase);

  std::ostream &operator()(std::ostream &os) const;

private:
  CGetElementSetInfo(const CGetElementSetInfo &rhs);
  CGetElementSetInfo &operator=(CGetElementSetInfo rhs);

  const CModelBase *m_modelBase;
};

} // namespace cora

std::ostream &operator<<(std::ostream &os, const cora::CGetElementSetInfo &i);

#endif // _cora_GetElementSetInfo_h_
