#ifndef _cora_GetHorizonInfo_h_
#define _cora_GetHorizonInfo_h_

#include <fstream>

class CModelBase;

#include "Object.h"

namespace cora
{

class CGetHorizonInfo
{
  public:
    static CGetHorizonInfo& instance(const CModelBase* modelBase);

    const TObjects getObjects() const;

    std::ostream& operator () (std::ostream& os) const;

  private:
    CGetHorizonInfo(const CModelBase* modelBase);
    ~CGetHorizonInfo();

    CGetHorizonInfo(const CGetHorizonInfo& rhs);
    CGetHorizonInfo& operator = (CGetHorizonInfo rhs);

    static TObjects getObjects(const CModelBase* modelBase);
    static void cleanup();

    static CGetHorizonInfo* m_getHorizonInfo;
    const CModelBase* m_modelBase;
    TObjects m_objects;
};

} // namespace cora

std::ostream& operator << (std::ostream& os, const cora::CGetHorizonInfo& i);

#endif  // _cora_GetHorizonInfo_h_
