#ifndef _cora_GetFormationInfo_h_
#define _cora_GetFormationInfo_h_

class CModelBase;

#include "Object.h"

namespace cora
{

class CGetFormationInfo
{
  public:
    static CGetFormationInfo& instance(CModelBase* modelBase);

    const TObjects getObjects() const;

    std::ostream& operator () (std::ostream& os) const;

  private:
    CGetFormationInfo(CModelBase* modelBase);
    ~CGetFormationInfo();

    CGetFormationInfo(const CGetFormationInfo& rhs);
    CGetFormationInfo& operator = (CGetFormationInfo rhs);

    static TObjects getObjects(CModelBase* modelBase);
    static void getObjects(TObjects& objects, CModelBase* modelBase,
      CFormationBase* formationBase);

    static bool hasElementSet(const CFormationBase* formationBase);
    static void cleanup();

    static CGetFormationInfo* m_getFormationInfo;
    CModelBase* m_modelBase;
    TObjects m_objects;
};

} // namespace cora

std::ostream& operator << (std::ostream& os, const cora::CGetFormationInfo& i);

#endif  // _cora_GetFormationInfo_h_
