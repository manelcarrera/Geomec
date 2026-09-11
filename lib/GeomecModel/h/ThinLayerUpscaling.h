#ifndef _THINLAYERUPSCALING_H_
#define _THINLAYERUPSCALING_H_

class CDepletionStage;

#include "Upscaling.h"
#include "GeomecModelVisitor.h"

class CFFMaterial;
class IValueComposite;

namespace geo {
  class IBody;
  class IPoint;
  class CMatrix;
}

class CThinLayerUpscalingNode : public IUpscalingNodeBase
{
public:
  CThinLayerUpscalingNode(CFemAppModel& model);
  CThinLayerUpscalingNode(const QString& strName, C3DModel& model);
  CThinLayerUpscalingNode(const CThinLayerUpscalingNode &rhs);
  virtual ~CThinLayerUpscalingNode();

  virtual int FactoryType() const;
  void Execute(IProgressBase& prog);
  bool CanExecute() const;

  geo::CValue GetLoadValue(const geo::IBody& body, const geo::IPoint& pt, const IValueComposite& loadcomposite) const;
  const geo::CMatrix& GetCMatrix(const CFFMaterial& mat, const geo::IBody& body) const;
  const geo::CMatrix& GetSMatrix(const geo::IBody& body, const CDepletionStage& stage) const;

  ACCEPT_GEOMECMODELVISITORS(VisitThinLayerUpscalingNode);

private:
  void BuildCMatrix(geo::IMatrix& mat, const CFFMaterial& ffmat) const;
public:
  bool IsTarget(const geo::IBody& body) const;
  void GetExpansionFactors(const CFFMaterial& mat, double& betan, double& betat) const;
  void BoundPoissonRatios(geo::IMatrix& matC) const;

private:
  typedef std::map<CFFMaterial, geo::CMatrix> TMat2MatrixMap;
  mutable TMat2MatrixMap m_mpMat2CMatrix;
  mutable TMat2MatrixMap m_mpMat2SMatrix;

public:
  typedef struct
  {
  double Et;
  double En;
  double nut;
  double nun;
  double G;
  double betat;
  double betan;
  } TUpscaledMatParams;

  typedef std::map<std::pair<CPillarPos, const CDepletionStage*>, TUpscaledMatParams> TMatParamsMap;

  typedef struct
  {
  double P;
  double T;
  double eps_t;
  double eps_n;
  } TUpscaledLoadParams;

  typedef std::map<std::pair<CPillarPos, const CDepletionStage*>, TUpscaledLoadParams> TLoadParamsMap;

  TMat2MatrixMap& getMat2SMatrix() {return m_mpMat2SMatrix;}
  TMat2MatrixMap& getMat2CMatrix() {return m_mpMat2CMatrix;}
};
#endif // _THINLAYERUPSCALING_H_
