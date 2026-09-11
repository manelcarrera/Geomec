#ifndef _cora_GetFaultInfo_h_
#define _cora_GetFaultInfo_h_

#include <fstream>
#include <set>

#include <QString>

namespace geo {

class CInterfaceElement;

} // namespace geo

class CHorizonBase;
class CDepletionStage;
class CModelBase;

#include "Object.h"

namespace cora {

class CGetFaultInfo {
public:
  static CGetFaultInfo &instance(CModelBase *modelBase);

  const TObjects getObjects() const;

  std::ostream &operator()(std::ostream &os) const;

private:
  CGetFaultInfo(CModelBase *modelBase);
  ~CGetFaultInfo();

  CGetFaultInfo(const CGetFaultInfo &rhs);
  CGetFaultInfo &operator=(CGetFaultInfo rhs);

  static TObjects getObjects(CModelBase *modelBase);
  static void cleanup();

  static CGetFaultInfo *m_getFaultInfo;
  CModelBase *m_modelBase;
  TObjects m_objects;
};

} // namespace cora

std::ostream &operator<<(std::ostream &os, const cora::CGetFaultInfo &i);

#endif // _cora_GetFaultInfo_h_
