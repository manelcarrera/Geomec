#include "FillMaterialParameterProxies.h"
#include "RpnMaterialParameterProxy.h"
#include "ValueTypeFactory.h"
#include "DepletionStage.h"
#include "TextTagUtils.h"
#include "rpnstack.h"
#include <string>
#include <set>
#include <vector>

CFillMaterialParameterProxies::CFillMaterialParameterProxies(
  CMeshBase& mesh, rpn::CRpnStack& rpnStack)
: m_materialParameterProxies(fillMaterialParameterProxies(mesh, rpnStack))
{
}

std::set<std::string> CFillMaterialParameterProxies::m_stProxyNames;
const std::set<std::string> &CFillMaterialParameterProxies::GetNames
( const CModelBase& model
)
{
  if (m_stProxyNames.size() == 0)
  {
  rpn::CRpnStack rpnStack;
  CFillMaterialParameterProxies
      proxies( const_cast<CMeshBase &>(model.Mesh()), rpnStack);
  std::vector <IRpnMaterialParameterProxy*> vcProxy= 
      proxies.getMaterialParameterProxies();

  for (int ii= 0; ii< vcProxy.size(); ++ii)
  {
      m_stProxyNames.insert( vcProxy[ii]->TextTag().toStdString() );
  }

  for (int ii= 0; ii< vcProxy.size(); ++ii)
  {
      delete vcProxy[ii];
  }
  }

  return m_stProxyNames;
}

// waij TFS 92432
//
IRpnMaterialParameterProxy * 
CFillMaterialParameterProxies::CreateMaterialParameterProxy
( const CModelBase& model
, const std::string &name
, rpn::CRpnStack& rpnStack
)
{
  IRpnMaterialParameterProxy *proxy= 0;
  std::string baseName;
  int timeStep;

  if ( CFillMaterialParameterProxies::GetNames(model).find( name) !=
       CFillMaterialParameterProxies::GetNames(model).end()
     )
  {
  proxy= CFillMaterialParameterProxies::CreateMaterialParameterProxy
      (model, name, 0, rpnStack);
  }
  else if ( SimpleSplitTextTag(name,baseName,timeStep ) && timeStep != -1) // aap_D1 -> aap,1
  {
  if ( CFillMaterialParameterProxies::GetNames(model).find( baseName) !=
         CFillMaterialParameterProxies::GetNames(model).end()
       )
  {
      const CDepletionStage *pStage= 
    model.FindDepletionStage(timeStep);

      assert(pStage); // we checked the user input, it should be there
      proxy= CFillMaterialParameterProxies::CreateMaterialParameterProxy
    (model, baseName, pStage, rpnStack);
  }
  }

  return proxy;
}

IRpnMaterialParameterProxy *
CFillMaterialParameterProxies::CreateMaterialParameterProxy
( const CModelBase& model
, const std::string &name
, const CDepletionStage *pStage
, rpn::CRpnStack& rpnStack
)
{
  rpn::CRpnStack newStack;
  CFillMaterialParameterProxies
  myProxies(const_cast<CMeshBase &>(model.Mesh()), newStack);

  IRpnMaterialParameterProxy *proxy= 0;
  std::vector <IRpnMaterialParameterProxy*> vcProxies= 
  myProxies.getMaterialParameterProxies();

  for ( std::vector <IRpnMaterialParameterProxy*>::iterator it=vcProxies.begin()
      ; it<  vcProxies.end()
      ; ++it
      )
  {
  IRpnMaterialParameterProxy *pr= *it; // assignment for debug

  if ( pr->TextTag().toStdString() == name )
  {
       proxy=
         dynamic_cast<IRpnMaterialParameterProxy *>(pr->Clone(rpnStack));

       if ( pStage)
       {
         proxy->FixedStage(true);
         proxy->TimeStep(pStage);
       }

       break;
  }
  } //for

  for (std::vector <IRpnMaterialParameterProxy*>::iterator it= vcProxies.begin()
      ; it< vcProxies.end()
      ; ++it
      )
  {
  delete *it;
  }

  return proxy;
}

std::vector <IRpnMaterialParameterProxy*>
  CFillMaterialParameterProxies::getMaterialParameterProxies() const
{
  return m_materialParameterProxies;
}

// private
//
std::vector <IRpnMaterialParameterProxy*>
CFillMaterialParameterProxies::fillMaterialParameterProxies(
  CMeshBase& mesh, rpn::CRpnStack& rpnStack)
{
  std::vector <IRpnMaterialParameterProxy*> materialParameterProxies;

  materialParameterProxies.push_back(new CRpnMaterialParameterProxy <TYoungsModulus> (mesh, rpnStack));
  materialParameterProxies.push_back(new CRpnMaterialParameterProxy <TYoungsModulusDecompaction> (mesh, rpnStack));
  materialParameterProxies.push_back(new CRpnMaterialParameterProxy <TPoissonsRatio> (mesh, rpnStack));
  materialParameterProxies.push_back(new CRpnMaterialParameterProxy <TPoissonsRatioDecompaction> (mesh, rpnStack));
  materialParameterProxies.push_back(new CRpnMaterialParameterProxy <TDensity> (mesh, rpnStack));
  materialParameterProxies.push_back(new CRpnMaterialParameterProxy <TCohesion> (mesh, rpnStack));
  materialParameterProxies.push_back(new CRpnMaterialParameterProxy <TFrictionAngle> (mesh, rpnStack));

  // Mantis 2517 wjrx

  materialParameterProxies.push_back(new CRpnMaterialParameterProxy <TBulkStiffness> (mesh, rpnStack));
  materialParameterProxies.push_back(new CRpnMaterialParameterProxy <TShearModulus> (mesh, rpnStack));
  materialParameterProxies.push_back(new CRpnMaterialParameterProxy <TPorosity> (mesh, rpnStack));
  materialParameterProxies.push_back(new CRpnMaterialParameterProxy <TInitFriction> (mesh, rpnStack));
  materialParameterProxies.push_back(new CRpnMaterialParameterProxy <THardening> (mesh, rpnStack));
  materialParameterProxies.push_back(new CRpnMaterialParameterProxy <TPreConsolidation> (mesh, rpnStack));
  materialParameterProxies.push_back(new CRpnMaterialParameterProxy <TCapShape> (mesh, rpnStack));
  materialParameterProxies.push_back(new CRpnMaterialParameterProxy <TDilatation> (mesh, rpnStack));
  materialParameterProxies.push_back(new CRpnMaterialParameterProxy <TLatRatioMax> (mesh, rpnStack));
  materialParameterProxies.push_back(new CRpnMaterialParameterProxy <TLatRatioMin> (mesh, rpnStack));
  materialParameterProxies.push_back(new CRpnMaterialParameterProxy <TAzimuth> (mesh, rpnStack));
  materialParameterProxies.push_back(new CRpnMaterialParameterProxy <TTensileStretch> (mesh, rpnStack));
  materialParameterProxies.push_back(new CRpnMaterialParameterProxy <TThermalExpansion> (mesh, rpnStack));

  // Mantis 2666

  materialParameterProxies.push_back(new CRpnMaterialParameterProxy <TElasticHardening> (mesh, rpnStack));
  materialParameterProxies.push_back(new CRpnMaterialParameterProxy <TSecondaryPreconsolidation> (mesh, rpnStack));
  materialParameterProxies.push_back(new CRpnMaterialParameterProxy <TSecondaryHardening> (mesh, rpnStack));
  materialParameterProxies.push_back(new CRpnMaterialParameterProxy <TUniaxialElasticCompressibility> (mesh, rpnStack));
  materialParameterProxies.push_back(new CRpnMaterialParameterProxy <TUniaxialPlasticCompressibility> (mesh, rpnStack));
  materialParameterProxies.push_back(new CRpnMaterialParameterProxy <TViscosity> (mesh, rpnStack));

  // For completeness

  materialParameterProxies.push_back(new CRpnMaterialParameterProxy <TAnisotropicShearModulus> (mesh, rpnStack));
  materialParameterProxies.push_back(new CRpnMaterialParameterProxy <TAnisotropicShearModulusDecomp> (mesh, rpnStack));
  materialParameterProxies.push_back(new CRpnMaterialParameterProxy <TApertureParameter> (mesh, rpnStack));
  materialParameterProxies.push_back(new CRpnMaterialParameterProxy <TCreepA1> (mesh, rpnStack));
  materialParameterProxies.push_back(new CRpnMaterialParameterProxy <TCreepA2> (mesh, rpnStack));
  materialParameterProxies.push_back(new CRpnMaterialParameterProxy <TCreepN1> (mesh, rpnStack));
  materialParameterProxies.push_back(new CRpnMaterialParameterProxy <TCreepN2> (mesh, rpnStack));
  materialParameterProxies.push_back(new CRpnMaterialParameterProxy <TCreepQR1> (mesh, rpnStack));
  materialParameterProxies.push_back(new CRpnMaterialParameterProxy <TCreepQR2> (mesh, rpnStack));
  materialParameterProxies.push_back(new CRpnMaterialParameterProxy <TDynamicShearStiffness> (mesh, rpnStack,true));
  materialParameterProxies.push_back(new CRpnMaterialParameterProxy <TDynamicUniaxialStiffness> (mesh, rpnStack,true));
  materialParameterProxies.push_back(new CRpnMaterialParameterProxy <TEquivalentPlasticStrain1> (mesh, rpnStack));
  materialParameterProxies.push_back(new CRpnMaterialParameterProxy <TEquivalentPlasticStrain2> (mesh, rpnStack));
  materialParameterProxies.push_back(new CRpnMaterialParameterProxy <TEquivalentPlasticStrain3> (mesh, rpnStack));
  materialParameterProxies.push_back(new CRpnMaterialParameterProxy <TFluidBulkModulus> (mesh, rpnStack));
  materialParameterProxies.push_back(new CRpnMaterialParameterProxy <TFluidDensity> (mesh, rpnStack));
  materialParameterProxies.push_back(new CRpnMaterialParameterProxy <TFluidThermalExpansionCoefficient> (mesh, rpnStack));
  materialParameterProxies.push_back(new CRpnMaterialParameterProxy <TFractStiffNormal> (mesh, rpnStack));
  materialParameterProxies.push_back(new CRpnMaterialParameterProxy <TFractStiffShear> (mesh, rpnStack));
  materialParameterProxies.push_back(new CRpnMaterialParameterProxy <THardCohesion1> (mesh, rpnStack));
  materialParameterProxies.push_back(new CRpnMaterialParameterProxy <THardCohesion2> (mesh, rpnStack));
  materialParameterProxies.push_back(new CRpnMaterialParameterProxy <THardCohesion3> (mesh, rpnStack));
  materialParameterProxies.push_back(new CRpnMaterialParameterProxy <THardFriction1> (mesh, rpnStack));
  materialParameterProxies.push_back(new CRpnMaterialParameterProxy <THardFriction2> (mesh, rpnStack));
  materialParameterProxies.push_back(new CRpnMaterialParameterProxy <THardFriction3> (mesh, rpnStack));
  materialParameterProxies.push_back(new CRpnMaterialParameterProxy <THighFractDens> (mesh, rpnStack));
  materialParameterProxies.push_back(new CRpnMaterialParameterProxy <THighFractDensAzi> (mesh, rpnStack));
  materialParameterProxies.push_back(new CRpnMaterialParameterProxy <THighFractDensIncl> (mesh, rpnStack));
  materialParameterProxies.push_back(new CRpnMaterialParameterProxy <TInitialFractureApertureHigh> (mesh, rpnStack,true));
  materialParameterProxies.push_back(new CRpnMaterialParameterProxy <TInitialFractureApertureInter> (mesh, rpnStack,true));
  materialParameterProxies.push_back(new CRpnMaterialParameterProxy <TInitialFractureApertureLow> (mesh, rpnStack,true));
  materialParameterProxies.push_back(new CRpnMaterialParameterProxy <TInterFractDens> (mesh, rpnStack));
  materialParameterProxies.push_back(new CRpnMaterialParameterProxy <TLayerNormalAzi> (mesh, rpnStack));
  materialParameterProxies.push_back(new CRpnMaterialParameterProxy <TLayerNormalIncl> (mesh, rpnStack));
  materialParameterProxies.push_back(new CRpnMaterialParameterProxy <TLowFractDens> (mesh, rpnStack));
  materialParameterProxies.push_back(new CRpnMaterialParameterProxy <TLowFractDensAzi> (mesh, rpnStack));
  materialParameterProxies.push_back(new CRpnMaterialParameterProxy <TLowFractDensIncl> (mesh, rpnStack));
  materialParameterProxies.push_back(new CRpnMaterialParameterProxy <TPermeability> (mesh, rpnStack));
  materialParameterProxies.push_back(new CRpnMaterialParameterProxy <TPoissonRatioNormal> (mesh, rpnStack));
  materialParameterProxies.push_back(new CRpnMaterialParameterProxy <TPoissonRatioNormalDecomp> (mesh, rpnStack));
  materialParameterProxies.push_back(new CRpnMaterialParameterProxy <TPoissonRatioTransDecomp> (mesh, rpnStack));
  materialParameterProxies.push_back(new CRpnMaterialParameterProxy <TPoissonRatioTransverse> (mesh, rpnStack));
  materialParameterProxies.push_back(new CRpnMaterialParameterProxy <TReferenceAperture> (mesh, rpnStack));
  materialParameterProxies.push_back(new CRpnMaterialParameterProxy <TReferenceStress> (mesh, rpnStack));
  materialParameterProxies.push_back(new CRpnMaterialParameterProxy <TRigidParam1> (mesh, rpnStack,true));
  materialParameterProxies.push_back(new CRpnMaterialParameterProxy <TRigidParam10> (mesh, rpnStack,true));
  materialParameterProxies.push_back(new CRpnMaterialParameterProxy <TRigidParam11> (mesh, rpnStack,true));
  materialParameterProxies.push_back(new CRpnMaterialParameterProxy <TRigidParam12> (mesh, rpnStack,true));
  materialParameterProxies.push_back(new CRpnMaterialParameterProxy <TRigidParam13> (mesh, rpnStack,true));
  materialParameterProxies.push_back(new CRpnMaterialParameterProxy <TRigidParam14> (mesh, rpnStack,true));
  materialParameterProxies.push_back(new CRpnMaterialParameterProxy <TRigidParam15> (mesh, rpnStack,true));
  materialParameterProxies.push_back(new CRpnMaterialParameterProxy <TRigidParam16> (mesh, rpnStack,true));
  materialParameterProxies.push_back(new CRpnMaterialParameterProxy <TRigidParam17> (mesh, rpnStack,true));
  materialParameterProxies.push_back(new CRpnMaterialParameterProxy <TRigidParam18> (mesh, rpnStack,true));
  materialParameterProxies.push_back(new CRpnMaterialParameterProxy <TRigidParam19> (mesh, rpnStack,true));
  materialParameterProxies.push_back(new CRpnMaterialParameterProxy <TRigidParam2> (mesh, rpnStack,true));
  materialParameterProxies.push_back(new CRpnMaterialParameterProxy <TRigidParam20> (mesh, rpnStack,true));
  materialParameterProxies.push_back(new CRpnMaterialParameterProxy <TRigidParam21> (mesh, rpnStack,true));
  materialParameterProxies.push_back(new CRpnMaterialParameterProxy <TRigidParam3> (mesh, rpnStack,true));
  materialParameterProxies.push_back(new CRpnMaterialParameterProxy <TRigidParam4> (mesh, rpnStack,true));
  materialParameterProxies.push_back(new CRpnMaterialParameterProxy <TRigidParam5> (mesh, rpnStack,true));
  materialParameterProxies.push_back(new CRpnMaterialParameterProxy <TRigidParam6> (mesh, rpnStack,true));
  materialParameterProxies.push_back(new CRpnMaterialParameterProxy <TRigidParam7> (mesh, rpnStack,true));
  materialParameterProxies.push_back(new CRpnMaterialParameterProxy <TRigidParam8> (mesh, rpnStack,true));
  materialParameterProxies.push_back(new CRpnMaterialParameterProxy <TRigidParam9> (mesh, rpnStack,true));
  materialParameterProxies.push_back(new CRpnMaterialParameterProxy <TThermLinExpLateral> (mesh, rpnStack));
  materialParameterProxies.push_back(new CRpnMaterialParameterProxy <TThermLinExpNormal> (mesh, rpnStack));
  materialParameterProxies.push_back(new CRpnMaterialParameterProxy <TThermalCapacity> (mesh, rpnStack));
  materialParameterProxies.push_back(new CRpnMaterialParameterProxy <TThermalConductivity> (mesh, rpnStack));
  materialParameterProxies.push_back(new CRpnMaterialParameterProxy <TThomsenDelta> (mesh, rpnStack));
  materialParameterProxies.push_back(new CRpnMaterialParameterProxy <TThomsenEpsilon> (mesh, rpnStack));
  materialParameterProxies.push_back(new CRpnMaterialParameterProxy <TThomsenGamma> (mesh, rpnStack));
  materialParameterProxies.push_back(new CRpnMaterialParameterProxy <TVelocityP> (mesh, rpnStack));
  materialParameterProxies.push_back(new CRpnMaterialParameterProxy <TVelocityS> (mesh, rpnStack));
  materialParameterProxies.push_back(new CRpnMaterialParameterProxy <TYoungModulusNormal> (mesh, rpnStack));
  materialParameterProxies.push_back(new CRpnMaterialParameterProxy <TYoungModulusTransDecomp> (mesh, rpnStack));
  materialParameterProxies.push_back(new CRpnMaterialParameterProxy <TYoungModulusTransverse> (mesh, rpnStack));
  materialParameterProxies.push_back(new CRpnMaterialParameterProxy <TYoungsModulusNormalDecomp> (mesh, rpnStack));

  // TFS, Bug 101563

  materialParameterProxies.push_back(new CRpnMaterialParameterProxy <TGrainStiffnessParameter> (mesh, rpnStack));

  return materialParameterProxies;
}
