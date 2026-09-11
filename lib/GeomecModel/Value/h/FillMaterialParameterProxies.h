#ifndef _FillMaterialParameterProxies_h_
#define _FillMaterialParameterProxies_h_

#include <vector>
#include <set>
#include <string>

class CDepletionStage;
class CMeshBase;
class IRpnMaterialParameterProxy;
class CModelBase;

namespace rpn
{

class CRpnStack;

} // namespace rpn

class CFillMaterialParameterProxies
{
public:
  CFillMaterialParameterProxies(CMeshBase& mesh, rpn::CRpnStack& rpnStack);

  static const std::set<std::string> &GetNames(const CModelBase &model);

  std::vector <IRpnMaterialParameterProxy*> getMaterialParameterProxies() const;

  static IRpnMaterialParameterProxy * CreateMaterialParameterProxy
  ( const CModelBase & model
  , const std::string &name
  , rpn::CRpnStack& rpnStack
  );

  static IRpnMaterialParameterProxy * CreateMaterialParameterProxy
  ( const CModelBase & model
  , const std::string &name
  , const CDepletionStage *pStage
  , rpn::CRpnStack& rpnStack
  );

private:
  CFillMaterialParameterProxies(const CFillMaterialParameterProxies& rhs);
  CFillMaterialParameterProxies& operator = (
  const CFillMaterialParameterProxies& rhs);

  static std::vector <IRpnMaterialParameterProxy*> fillMaterialParameterProxies(
  CMeshBase& mesh, rpn::CRpnStack& rpnStack);

  std::vector <IRpnMaterialParameterProxy*> m_materialParameterProxies;
  static std::set<std::string> m_stProxyNames;
};

#endif  // _FillMaterialParameterProxies_h_
