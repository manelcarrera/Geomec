#ifndef _cora_GetWellPathInfo_h_
#define _cora_GetWellPathInfo_h_

#include <fstream>

class CModelBase;

#include "Object.h"

namespace cora
{

class CGetWellPathInfo
{
  public:
    static CGetWellPathInfo& instance(const CModelBase* modelBase);

    const TObjects getObjects() const;

    std::ostream& operator () (std::ostream& os) const;

  private:
    CGetWellPathInfo(const CModelBase* modelBase);
    ~CGetWellPathInfo();

    CGetWellPathInfo(const CGetWellPathInfo& rhs);
    CGetWellPathInfo& operator = (CGetWellPathInfo rhs);

    static TObjects getObjects(const CModelBase* modelBase);
    static void cleanup();

    static CGetWellPathInfo* m_getWellPathInfo;
    const CModelBase* m_modelBase;
    TObjects m_objects;
};

} // namespace cora

std::ostream& operator << (std::ostream& os, const cora::CGetWellPathInfo& i);

#endif  // _cora_GetWellPathInfo_h_
