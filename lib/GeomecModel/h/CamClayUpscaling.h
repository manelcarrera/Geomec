#ifndef _CAMCLAYUPSCALING_H_
#define _CAMCLAYUPSCALING_H_

#include "Upscaling.h"
#include "FormationBase.h"
#include "IFormationFilter.h"
#include "Materials.h"
#include "GeomecModelVisitor.h"

class CCamClayFormationFilter : public IFormationFilter
{
public:
  virtual bool FormationValid(const CFormationBase& formation) const
  {
    switch(formation.ActiveMaterial().MaterialModel())
    {
    case MM_LINEAR:
    case MM_CAMCLAY:
    case MM_MOHRCOULOMB:
    case MM_UNDRAINED:
    case MM_DUALCAP_LINELA:
      return true;
    default:
      return false;
    }
  }
};

class CCamClayUpscalingNode : public IUpscalingNodeBase
{
public:
	CCamClayUpscalingNode(CFemAppModel& model);
	CCamClayUpscalingNode(const QString& strName, C3DModel& model);
	CCamClayUpscalingNode(const CCamClayUpscalingNode &rhs);
	virtual ~CCamClayUpscalingNode(); 

  virtual int FactoryType() const;

  virtual void LoadStream(TSTREAM& stream, CStreamVersion& version, TPROGRESS& progress);

  virtual void Execute(IProgressBase& prog);
  virtual bool CanExecute() const;
private:
  CCamClayFormationFilter *m_formationFilter;

public:
  typedef struct
  {
    double Ce;    // elastic compressibility 
    double nu;    // Poisson ratio
    double E;     // Young's modulus
    double n0;    // porosity
    double Cp1;   // plastic compressibility 1
    double Cp2;   // plastic compressibility 2
    double Pc1;   // Preconsolidation stress 1
    double Pc2;   // Presonsolidation stress 2
    double alpha; // capshape  
    double gamma; // Tensile stretch
    double KH;    // SHtot/Stot ratio
    double Kh;    // Shtot/Svtot ratio
    double OH;    // SH azimuth
    double Phi;   // friction angle
    double Lam1;  // Hardening coeff 1
    double Lam2;  // Hardening coeff 2
    double Rho;   // density
    double Beta;  // thermal expansion coeff
    double Et;    // transverse Young's Modulus
    double En;    // normal Young's Modulus
    double nut;   // transverse Poisson's ratio
    double nun;   // normal Poisson's ratio
    double Ga;    // anisotropic shear modulus
  }TUpscaledParams;

  typedef std::map<std::pair<CPillarPos, const CDepletionStage*>, TUpscaledParams> TMatParamsMap;

  typedef struct
  {
    double P; // Pore pressure
    double T; // Temperature
  } TUpscaledLoadParams;

  typedef std::map<std::pair<CPillarPos, const CDepletionStage*>, TUpscaledLoadParams> TLoadParamsMap;

private:
  double MaterialValue(const geo::IBody &body, const CDepletionStage &stage, unsigned int parameterID);
  bool ParameterExists(const geo::IBody &body, const CDepletionStage &stage, unsigned int parameterID);
public:
  bool IsTarget(const geo::IBody& body) const;
  bool IsCCBody(const geo::IBody &body, const CDepletionStage &stage) const;
private:
  bool IsDualCap(const geo::IBody &body, const CDepletionStage &stage) const;



  // some temp values to work with
  // These need to be calculated:

  typedef struct
  {
    double Cei;
    double Cp1i;
    double Cp2i;
    double psii;
    double psib;
    double Kp0;
  } TCalculatedValues;

  TCalculatedValues calculatedValues;

  // These are material model parameters:

  typedef struct
  {
    double Nui;
    double Ei;
    double Pc1i;
    double Pc2i;
    double alphai;
    double gammai;
    double KHi;
    double Khi;
    double OHi;
    double Phii;
    double Lam1i;
    double Lam2i;
    double Rhoi;
    double Betai;
    double n0i;
  } TMaterialModelParameters;

  TMaterialModelParameters materialModelParameters;

public:
  void FillNonCCValues(const geo::IBody &body, const CDepletionStage &stage);
  void FillCCValues(const geo::IBody &body, const CDepletionStage &stage);
  TUpscaledParams& UpscaledParameters(TMatParamsMap &mpParams, const CDepletionStage &stage, const CPillarPos &pos);
  geo::CValue GetLoadValue(const geo::IBody& body, const geo::IPoint& pt, const IValueComposite& loadcomposite) const;
  double ElasticCompressibility(const double &nu, const double &E);
  double PlasticCompressibility1(const double &n0, const double &Lam1,
                                const double &Pc1, const double &Ce, 
                                const double &alpha, const double &phi);
  double PlasticCompressibility2(const double &n0, const double &Lam1,
                                const double &Lam2, const double &Pc2,
                                const double &Ce, const double &alpha,
                                const double &phi);
  TCalculatedValues& getCalculatedValues() {return calculatedValues;}
  TMaterialModelParameters& getMaterialModelParameters() {return materialModelParameters;}

  ACCEPT_GEOMECMODELVISITORS(VisitCamClayUpscalingNode);
};

#endif