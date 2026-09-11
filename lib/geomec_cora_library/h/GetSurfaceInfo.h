#ifndef _cora_GetSurfaceInfo_h_
#define _cora_GetSurfaceInfo_h_

#include <fstream>

class CModelBase;

#include "Object.h"

namespace cora {

class CGetSurfaceInfo {
public:
  static CGetSurfaceInfo &instance(const CModelBase *modelBase);

  const TObjects getObjects() const;

  std::ostream &operator()(std::ostream &os) const;

private:
  CGetSurfaceInfo(const CModelBase *modelBase);
  ~CGetSurfaceInfo();

  CGetSurfaceInfo(const CGetSurfaceInfo &rhs);
  CGetSurfaceInfo &operator=(CGetSurfaceInfo rhs);

  static TObjects getObjects(const CModelBase *modelBase);
  static void cleanup();

  static CGetSurfaceInfo *m_getSurfaceInfo;
  const CModelBase *m_modelBase;
  TObjects m_objects;
};

} // namespace cora

std::ostream &operator<<(std::ostream &os, const cora::CGetSurfaceInfo &i);

#endif // _cora_GetSurfaceInfo_h_
