#ifndef _cora_GetNonMeshedSurfaceInfo_h_
#define _cora_GetNonMeshedSurfaceInfo_h_

#include "Object.h"

namespace cora {

class CGetNonMeshedSurfaceInfo {
public:
  static CGetNonMeshedSurfaceInfo &instance(CModelBase *modelBase);

  const TObjects getObjects() const;

  std::ostream &operator()(std::ostream &os) const;

private:
  CGetNonMeshedSurfaceInfo(CModelBase *modelBase);
  ~CGetNonMeshedSurfaceInfo();

  CGetNonMeshedSurfaceInfo(const CGetNonMeshedSurfaceInfo &rhs);
  CGetNonMeshedSurfaceInfo &operator=(CGetNonMeshedSurfaceInfo rhs);

  static TObjects getObjects(CModelBase *modelBase);
  static void cleanup();

  static CGetNonMeshedSurfaceInfo *m_getNonMeshedSurfaceInfo;
  CModelBase *m_modelBase;
  TObjects m_objects;
};

} // namespace cora

std::ostream &operator<<(std::ostream &os, const cora::CGetNonMeshedSurfaceInfo &i);

#endif // _cora_GetNonMeshedSurfaceInfo_h_
