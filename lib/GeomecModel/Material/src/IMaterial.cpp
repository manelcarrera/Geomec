// IMaterial.cpp: implementation of the IMaterial class.
//
//////////////////////////////////////////////////////////////////////


#include "IMaterial.h"
#include "MaterialEntry.h"
#include "MaterialServer.h"
#include "ModelBase.h"
#include "ValueTypes.h"
#include "DianaRunController.h"
#include "lbfl.h"
#include "LibraryMaterial.h"
#include "MaterialParameter.h"
#include "StreamVersion.h"
#include "GeomecDianaRunner.h"
#include "Material.h"
#include "Materials.h"
#include "MaterialFactory.h"

#include "GlobalConstants.h"

const QString MLD_ELASTIC_HARDENING_OLD = "Elastic Hardening Coefficient";
const QString MLD_SEC_HARDENING_OLD     = "Secondary Hardening Coefficient";
const QString MLD_RIGIDITY_PARAM1_OLD   = "Kxxxx";
const QString MLD_RIGIDITY_PARAM2_OLD   = "Kxxyy";
const QString MLD_RIGIDITY_PARAM3_OLD   = "Kxxzz";
const QString MLD_RIGIDITY_PARAM4_OLD   = "Kxxxy";
const QString MLD_RIGIDITY_PARAM5_OLD   = "Kxxyz";
const QString MLD_RIGIDITY_PARAM6_OLD   = "Kxxzx";
const QString MLD_RIGIDITY_PARAM7_OLD   = "Kyyyy";
const QString MLD_RIGIDITY_PARAM8_OLD   = "Kyyzz";
const QString MLD_RIGIDITY_PARAM9_OLD   = "Kyyxy";
const QString MLD_RIGIDITY_PARAM10_OLD  = "Kyyyz";
const QString MLD_RIGIDITY_PARAM11_OLD  = "Kyyzx";
const QString MLD_RIGIDITY_PARAM12_OLD  = "Kzzzz";
const QString MLD_RIGIDITY_PARAM13_OLD  = "Kzzxy";
const QString MLD_RIGIDITY_PARAM14_OLD  = "Kzzyz";
const QString MLD_RIGIDITY_PARAM15_OLD  = "Kzzzx";
const QString MLD_RIGIDITY_PARAM16_OLD  = "Kxyxy";
const QString MLD_RIGIDITY_PARAM17_OLD  = "Kxyyz";
const QString MLD_RIGIDITY_PARAM18_OLD  = "Kxyzx";
const QString MLD_RIGIDITY_PARAM19_OLD  = "Kyzyz";
const QString MLD_RIGIDITY_PARAM20_OLD  = "Kyzzx";
const QString MLD_RIGIDITY_PARAM21_OLD  = "Kzxzx";
const QString MLD_APERTUREPARAM_OLD     = "Aperture Parameter";
const QString MLD_SHEARMODULUS_DECOMP   = "Shear Modulus in decompaction";
const QString MLD_ANISOTROPIC_VELOCITYP = "Anisotropic Vp";
const QString MLD_ANISOTROPIC_VELOCITYS = "Anisotropic Vs";

// waij TFS 81578 shortened names
const QString MLD_UNDRAINEDTHERMX_OLD   = "Undrained Thermal Expansion Coefficient";
const QString MLD_THERMX_OLD            = "Volumetric Thermal Expansion Coefficient";
const QString MLD_HIGH_FRACT_DENS_INCL_OLD = "Highest Fracture Density inclination";
const QString MLD_LOW_FRACT_DENS_INCL_OLD = "Lowest Fracture Density inclination";
const QString MLD_YOUNGMODULUS_NORM_DECOMP_OLD  = "Young's Modulus normal in decompaction";
const QString MLD_YOUNGMODULUS_TRANS_DECOMP_OLD = "Young's Modulus transverse in decompaction";
const QString MLD_POISSONRATIO_NORM_DECOMP_OLD  = "Poisson's Ratio normal in decompaction";
const QString MLD_POISSONRATIO_TRANS_DECOMP_OLD = "Poisson's Ratio transverse in decompaction";
const QString MLD_ANISOTROPIC_SHEARMODULUS_DECOMP_OLD = "Anisotropic Shear Modulus in decompaction";
const QString MLD_V0_HI_OLD = "V0 in highest fracture density direction";
const QString MLD_V0_ME_OLD = "V0 in intermediate fracture density direction";
const QString MLD_V0_LO_OLD = "V0 in lowest fracture denstiy direction";
const QString MLD_FLUIDX_OLD = "Fluid Thermal Expansion Coefficient";




static QString ConvertMatParamName(const QString& name)
{
  // convert a renamed parameter name from the old
  // value (from the file) to the new value
  typedef std::map<QString, QString> TNameMap;
  static TNameMap mpNames;

  if(mpNames.empty())
  {
  mpNames.insert(TNameMap::value_type(MLD_ELASTIC_HARDENING_OLD, MLD_ELASTIC_HARDENING));
  mpNames.insert(TNameMap::value_type(MLD_SEC_HARDENING_OLD, MLD_SEC_HARDENING));
  mpNames.insert(TNameMap::value_type(MLD_RIGIDITY_PARAM1_OLD, MLD_RIGIDITY_PARAM1));
  mpNames.insert(TNameMap::value_type(MLD_RIGIDITY_PARAM2_OLD, MLD_RIGIDITY_PARAM2));
  mpNames.insert(TNameMap::value_type(MLD_RIGIDITY_PARAM3_OLD, MLD_RIGIDITY_PARAM3));
  mpNames.insert(TNameMap::value_type(MLD_RIGIDITY_PARAM4_OLD, MLD_RIGIDITY_PARAM4));
  mpNames.insert(TNameMap::value_type(MLD_RIGIDITY_PARAM5_OLD, MLD_RIGIDITY_PARAM5));
  mpNames.insert(TNameMap::value_type(MLD_RIGIDITY_PARAM6_OLD, MLD_RIGIDITY_PARAM6));
  mpNames.insert(TNameMap::value_type(MLD_RIGIDITY_PARAM7_OLD, MLD_RIGIDITY_PARAM7));
  mpNames.insert(TNameMap::value_type(MLD_RIGIDITY_PARAM8_OLD, MLD_RIGIDITY_PARAM8));
  mpNames.insert(TNameMap::value_type(MLD_RIGIDITY_PARAM9_OLD, MLD_RIGIDITY_PARAM9));
  mpNames.insert(TNameMap::value_type(MLD_RIGIDITY_PARAM10_OLD, MLD_RIGIDITY_PARAM10));
  mpNames.insert(TNameMap::value_type(MLD_RIGIDITY_PARAM11_OLD, MLD_RIGIDITY_PARAM11));
  mpNames.insert(TNameMap::value_type(MLD_RIGIDITY_PARAM12_OLD, MLD_RIGIDITY_PARAM12));
  mpNames.insert(TNameMap::value_type(MLD_RIGIDITY_PARAM13_OLD, MLD_RIGIDITY_PARAM13));
  mpNames.insert(TNameMap::value_type(MLD_RIGIDITY_PARAM14_OLD, MLD_RIGIDITY_PARAM14));
  mpNames.insert(TNameMap::value_type(MLD_RIGIDITY_PARAM15_OLD, MLD_RIGIDITY_PARAM15));
  mpNames.insert(TNameMap::value_type(MLD_RIGIDITY_PARAM16_OLD, MLD_RIGIDITY_PARAM16));
  mpNames.insert(TNameMap::value_type(MLD_RIGIDITY_PARAM17_OLD, MLD_RIGIDITY_PARAM17));
  mpNames.insert(TNameMap::value_type(MLD_RIGIDITY_PARAM18_OLD, MLD_RIGIDITY_PARAM18));
  mpNames.insert(TNameMap::value_type(MLD_RIGIDITY_PARAM19_OLD, MLD_RIGIDITY_PARAM19));
  mpNames.insert(TNameMap::value_type(MLD_RIGIDITY_PARAM20_OLD, MLD_RIGIDITY_PARAM20));
  mpNames.insert(TNameMap::value_type(MLD_RIGIDITY_PARAM21_OLD, MLD_RIGIDITY_PARAM21));
  mpNames.insert(TNameMap::value_type(MLD_APERTUREPARAM_OLD, MLD_APERTUREPARAM));
  mpNames.insert(TNameMap::value_type(MLD_SHEARMODULUS_DECOMP, MLD_ANISOTROPIC_SHEARMODULUS_DECOMP));
  mpNames.insert(TNameMap::value_type(MLD_ANISOTROPIC_VELOCITYP, MLD_VELOCITYP));
  mpNames.insert(TNameMap::value_type(MLD_ANISOTROPIC_VELOCITYS, MLD_VELOCITYS));

  mpNames.insert(TNameMap::value_type(MLD_UNDRAINEDTHERMX_OLD, MLD_UNDRAINEDTHERMX));
  mpNames.insert(TNameMap::value_type(MLD_THERMX_OLD, MLD_THERMX));
  mpNames.insert(TNameMap::value_type(MLD_HIGH_FRACT_DENS_INCL_OLD, MLD_HIGH_FRACT_DENS_INCL));
  mpNames.insert(TNameMap::value_type(MLD_LOW_FRACT_DENS_INCL_OLD, MLD_LOW_FRACT_DENS_INCL));
  mpNames.insert(TNameMap::value_type(MLD_YOUNGMODULUS_NORM_DECOMP_OLD, MLD_YOUNGMODULUS_NORM_DECOMP));
  mpNames.insert(TNameMap::value_type(MLD_YOUNGMODULUS_TRANS_DECOMP_OLD, MLD_YOUNGMODULUS_TRANS_DECOMP));
  mpNames.insert(TNameMap::value_type(MLD_POISSONRATIO_NORM_DECOMP_OLD, MLD_POISSONRATIO_NORM_DECOMP));
  mpNames.insert(TNameMap::value_type(MLD_POISSONRATIO_TRANS_DECOMP_OLD, MLD_POISSONRATIO_TRANS_DECOMP));
  mpNames.insert(TNameMap::value_type(MLD_ANISOTROPIC_SHEARMODULUS_DECOMP_OLD, MLD_ANISOTROPIC_SHEARMODULUS_DECOMP));
  mpNames.insert(TNameMap::value_type(MLD_V0_HI_OLD, MLD_V0_HI));
  mpNames.insert(TNameMap::value_type(MLD_V0_ME_OLD, MLD_V0_ME));
  mpNames.insert(TNameMap::value_type(MLD_V0_LO_OLD, MLD_V0_LO));
  mpNames.insert(TNameMap::value_type(MLD_FLUIDX_OLD, MLD_FLUIDX));
  }

  TNameMap::iterator it = mpNames.find(name);
  if(it != mpNames.end())
  return it->second;

  return name;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMaterial::IMaterial(CMaterialEntry &entry, CLibraryMaterial& libmat)
: IMaterialBase("", entry.Model()),
  m_entry(entry),
  m_LibraryMaterial(libmat),
  m_observer(*this, libmat)
{
  CreateParameters();
}

IMaterial::~IMaterial()
{
  DestroyParameters();
}

/*
IMaterial& IMaterial::operator =(const IMaterial& rhs)
{
  assert(m_mpParameters.size() == rhs.m_mpParameters.size());

  Name(rhs.Name());

  // copy the number of iterations and the convergention criterium in the lib.
  m_entry.Library().SetMaxNumIterations(Name().toStdString().c_str(), rhs.m_entry.Library().GetMaxNumIterations(Name().toStdString().c_str()));
  m_entry.Library().SetConvCriterion(Name().toStdString().c_str(), rhs.m_entry.Library().GetConvCriterion(Name().toStdString().c_str()));

  // Copy parameters in matlibX
  // Iterate over the parameters in the newly created material in MatLibX, and see if the given parameter's name
  // can be found in the Filos file. If so, set its value and fixed state in the MatLibX library.
  // If a parameter's name is not found in the Filos file, it is left unchanged (default) in the MatLibX library.
  for(int i = 0; i < m_entry.Library().GetMatParamCount(Name().toStdString().c_str()); i++)
  {
    // Copy the parameters of the material
    QString sParamName = m_entry.Library().GetMatParamName(Name().toStdString().c_str(), i);
    m_entry.Library().GetParameter(Name().toStdString().c_str(), sParamName)->SetValue(rhs.m_entry.Library().GetParameter(Name().toStdString().c_str(), sParamName)->GetValue());
    m_entry.Library().GetParameter(Name().toStdString().c_str(), sParamName)->SetFixed(rhs.m_entry.Library().GetParameter(Name().toStdString().c_str(), sParamName)->GetFixed());		

    // Create an filos parameters!!!!
    double dValue = m_entry.Library().GetParameter(Name().toStdString().c_str(), sParamName)->GetValue().dblVal;
    unsigned int uiValueTypeID;
    VERIFY(CMaterialParameter::ValueTypeIDFromMatLibXID(std::string(sParamName), uiValueTypeID));
    TParameterMap::iterator it = m_mpParameters.find(uiValueTypeID);
    assert(it != m_mpParameters.end());
    it->second.Value(dValue);
    assert(it->second.MatLibXString() == sParamName);
  }

  return *this;
}
*/
void IMaterial::CreateParameters()
{
  for(size_t i = 0; i < m_LibraryMaterial.ParameterSize(); ++i)
  {
  CLibraryMaterialParameter& param = m_LibraryMaterial.Parameter(i);
    unsigned int uiValueTypeID;
    bool succeeded = false;
    succeeded = CMaterialParameter::ValueTypeIDFromMatLibXID(param.Name(), uiValueTypeID);
    assert(succeeded);
    succeeded = m_mpParameters.insert(TParameterMap::value_type(&param, new CMaterialParameter(uiValueTypeID, param))).second;
    assert(succeeded);
  }
/*
  int nParams = m_entry.Library().GetMatParamCount(Name());
  assert(nParams > 0);

  for(int i = 0; i < nParams; i++)
  {
    QString ParamName = m_entry.Library().GetMatParamName(Name().toStdString().c_str(), i);
    CMatParam *Param = m_entry.Library().GetParameter(Name().toStdString().c_str(), ParamName);
    VARIANT va = Param->GetValue();
    assert(va.vt == VT_R8);
    double dValue = va.dblVal;
    unsigned int uiValueTypeID;
    VERIFY(CMaterialParameter::ValueTypeIDFromMatLibXID(std::string(ParamName), uiValueTypeID));

    m_mpParameters.insert(TParameterMap::value_type(uiValueTypeID, CMaterialParameter(dValue, uiValueTypeID, ParamName)));
  }
*/
}

void IMaterial::DestroyParameters()
{
  for(TParameterMap::iterator it = m_mpParameters.begin(); it != m_mpParameters.end(); ++it)
  delete it->second;

  m_mpParameters.clear();
}

unsigned int IMaterial::TypeId() const
{
  return 0;
}

bool IMaterial::Destroy()
{
  assert(CanDestroy());
  m_entry.DeleteMaterial(*this);
/*
  m_entry.Library().Reset();
  if(m_entry.Library().DeleteMaterial(Name().toStdString().c_str()))
    delete this;
  else
    _m()->msg("Cannot delete material");
*/
  return true;
}

bool IMaterial::CanDestroy() const
{
  return !IsLocked();
}

bool IMaterial::IsLocked() const
{
  return false;
}

bool IMaterial::Valid() const
{
  return true;
}

/*
// in this function: std::map<QString, std::pair<double, bool>, TnoCaseStringCompare> : QString = name of paramter
// std::pair<double, bool> Value of parameter and Fixed or not. 
void IMaterial::AddMaterialToLibrary(const QString strName, int nModel, 
                   std::map<QString, std::pair<double, bool>, TnoCaseStringCompare> mpParam,
                   int nMaxIterations, double dConvCriterion)
{
  m_entry.Library().AddMaterial(strName, nModel);
  m_entry.Library().SetMaxNumIterations(strName, nMaxIterations);
  m_entry.Library().SetConvCriterion(strName, dConvCriterion);

  int i;
  // parameters
  // Iterate over the parameters in the newly created material in MatLibX, and see if the given parameter's name
  // can be found in the Filos file. If so, set its value and fixed state in the MatLibX library.
  // If a parameter's name is not found in the Filos file, it is left unchanged (default) in the MatLibX library.
  for(i = 0; i < m_entry.Library().GetMatParamCount(Name().toStdString().c_str()); i++)
  {
    // Try to find parameter name in our map ...
    std::map<QString, std::pair<double, bool>, TnoCaseStringCompare>::iterator it = mpParam.find(m_entry.Library().GetMatParamName(Name().toStdString().c_str(), i));
    if(it != mpParam.end())
    {
      // found the parameter, get the value map (from file) and set (in MatLibX) the value and fixed state now
      VARIANT vaValue;
      vaValue.vt= VT_R8;
      vaValue.dblVal = it->second.first;
      m_entry.Library().GetParameter(Name().toStdString().c_str(), it->first)->SetValue(vaValue);
      m_entry.Library().GetParameter(Name().toStdString().c_str(), it->first)->SetFixed(it->second.second);
    }
    
    // Create an filos parameters!!!!
    QString strParamName = m_entry.Library().GetMatParamName(Name().toStdString().c_str(), i);
    double dValue = m_entry.Library().GetParameter(Name().toStdString().c_str(), strParamName)->GetValue().dblVal;

    unsigned int uiValueTypeID;
    VERIFY(CMaterialParameter::ValueTypeIDFromMatLibXID(std::string(strParamName), uiValueTypeID));
    m_mpParameters.insert(TParameterMap::value_type(uiValueTypeID, CMaterialParameter(dValue, uiValueTypeID, strParamName)));
  }

  // material is initialized, link to the material entry
  reParent(&m_entry);
}
*/
void IMaterial::LoadStream(TSTREAM& stream, CStreamVersion& version, TPROGRESS& progress)
{
  typedef std::pair<double, bool> TParamValue;
  typedef std::map<QString, TParamValue> TParamMap;

//  if(IsLinkedTo(m_entry))
//    UnLink(m_entry);

  // Get name and index ...
  CStorageNode::LoadStream(stream, version, progress);

  // Set job name
  QString strJob;
  strJob = QString("Loading material %1").arg(Name());
  progress.StatusMessage(strJob);

  if(version < CStreamVersion(3, 8, 1))
  {
    // Read and create material model
    int nModel;
    stream >> nModel;
    //m_entry.Library().AddMaterial(Name(), nModel);
  }

  // Store max iterations for the material
  int nMaxIterations;
  stream >> nMaxIterations;
  //m_entry.Library().SetMaxNumIterations(Name(), nMaxIterations);
  progress.Step();

  // Store convergence crit for for the material
  double dConvCriterion;
  stream >> dConvCriterion;
  //m_entry.Library().SetConvCriterion(Name(), dConvCriterion);
  progress.Step();

  // Read parameter and put them in a map ...
  TParamMap mpParam;
  int nParamSize;
  stream >> nParamSize;
  for(int i = 0; i < nParamSize; i++)
  {
    QString sName;
    double dValue;
    int bFixed;
    stream >> sName;

  // special case for renamed parameters
  sName = ConvertMatParamName(sName);

    stream >> dValue;
    stream >> bFixed;
    mpParam.insert(TParamMap::value_type(sName, TParamValue(dValue, (bool)bFixed)));
    progress.Step();
  }

  if(version < CStreamVersion(3, 0, 123))
  {
  // convert the unit from cm2 to mD (=1e-15 m2)
  TParamMap::iterator it = mpParam.find(MLD_PERMEA);
  if(it != mpParam.end())
      it->second.first *= 1e13;
  }

  if(mpParam.find(MLD_YOUNGMODULUS_DECOMP) == mpParam.end())
  {
  TParamMap::iterator it = mpParam.find(MLD_YOUNGMODULUS);
  if(it != mpParam.end())
      mpParam.insert(TParamMap::value_type(MLD_YOUNGMODULUS_DECOMP, TParamValue(it->second.first, false)));
  }

  if(mpParam.find(MLD_POISSONRATIO_DECOMP) == mpParam.end())
  {
  TParamMap::iterator it = mpParam.find(MLD_POISSONRATIO);
  if(it != mpParam.end())
      mpParam.insert(TParamMap::value_type(MLD_POISSONRATIO_DECOMP, TParamValue(it->second.first, false)));
  }

  m_LibraryMaterial.Name(Name());
  m_LibraryMaterial.ConvCriterion(dConvCriterion);
  m_LibraryMaterial.MaxIterations(nMaxIterations);
  TParamMap::iterator it;
  for(it = mpParam.begin(); it != mpParam.end(); ++it)
  {
  CLibraryMaterialParameter* pParam = m_LibraryMaterial.Parameter(it->first);
  if(pParam)
  {
      pParam->CurrentlyFixed(it->second.second);
      pParam->LoadValue(it->second.first);
  }
  }

  // trigger dependencies
  m_LibraryMaterial.Finalize();
}

void IMaterial::SaveStream(TSTREAM& stream, TPROGRESS& progress)
{
  assert(!Empty());

  // Save index and name
  CStorageNode::SaveStream(stream, progress);

  // Set the job text
  QString strJob;
  strJob = QString("Saving material %1").arg(Name());
  progress.StatusMessage(strJob);

  // Max iterations
  int nMaxIterations = m_LibraryMaterial.MaxIterations();
  stream << nMaxIterations;
  progress.Step();

  // Conv Criterion
  double dConvCrit = m_LibraryMaterial.ConvCriterion();
  stream << dConvCrit;
  progress.Step();

  // parameters
  int nSize = m_LibraryMaterial.ParameterSize();
  stream << nSize;
  size_t i;
  for(i = 0; i < nSize; ++i)
  {
  const CLibraryMaterialParameter& param = m_LibraryMaterial.Parameter(i);
  stream << param.Name();
  stream << param.Value();
  int bFixed = (param.IsCurrentlyFixed() != false);
  stream << bFixed;
  }
}


long IMaterial::SavedItems() const
{
  return CStorageNode::SavedItems() + m_mpParameters.size() + 2;
}

bool IMaterial::Empty() const
{
  return !IsLinkedTo(m_entry);
}

const CMaterialParameter *IMaterial::Parameter(unsigned int ValueTypeID) const
{
  assert(!Empty());
  QString sParamName;
  CMaterialParameter::MatLibXIDFromValueTypeID(ValueTypeID, sParamName);
  TParameterMap::const_iterator it = m_mpParameters.find(m_LibraryMaterial.Parameter(sParamName));
  if(it != m_mpParameters.end())
    return it->second;

  return 0;
}
/*
void IMaterial::ConfigureReadOnlyParametersInLibrary() TODO wedx
{
  CMatLib& lib = m_entry.Library();

  int i;
  for(i = 0; !CMaterialServer::FixedParameters[i].param.empty(); ++i)
  {
  if(CMaterialServer::FixedParameters[i].model == MaterialModel() || CMaterialServer::FixedParameters[i].model == -1)
      lib.SetParameterReadOnlyForMaterial(Name().toStdString().c_str(), CMaterialServer::FixedParameters[i].param.c_str());
  }
}

void IMaterial::ConfigurePermanentReadOnlyParametersInLibrary() TODO wedx
{
  CMatLib& lib = m_entry.Library();

  // to circumvent update problems with decompaction parameters from dependent parameters
  lib.SetParameterReadOnly(MLD_BULKSTIFFNESS);
  lib.SetParameterReadOnly(MLD_DYNUNISTIFFNESS);
  lib.SetParameterReadOnly(MLD_DYNSHEARSTIFFNESS);
  lib.SetParameterReadOnly(MLD_VELOCITYP);
  lib.SetParameterReadOnly(MLD_VELOCITYS);
  lib.SetParameterReadOnly(MLD_UNDRAINEDYOUNG);
  lib.SetParameterReadOnly(MLD_UNDRAINEDPOISSON);
  lib.SetParameterReadOnly(MLD_UNDRAINEDTHERMX);

  if(MaterialModel() != MM_FRACTURE_ANISOTROPY && MaterialModel() != MM_UPSCALED_ANISOTROPY && MaterialModel() != MM_FRACTURE_APERTURE)
  lib.SetParameterReadOnly(MLD_SHEARMODULUS);
}

const CMaterialParameter *IMaterial::Parameter(std::string MatLibXID) const
{
  assert(!Empty());
  unsigned int ValueTypeID;
  VERIFY(CMaterialParameter::ValueTypeIDFromMatLibXID(MatLibXID, ValueTypeID));
  return Parameter(ValueTypeID);
}
*/
double IMaterial::ParameterValue(unsigned int ValueTypeID) const
{
  if (Parameter(ValueTypeID))
    return Parameter(ValueTypeID)->Value();
  else return 1e-20;//wjrx mantis 2529
}

double IMaterial::ParameterValue(const QString& MatLibXID) const
{
  unsigned int ValueTypeID;
  bool succeeded = false;
  succeeded = CMaterialParameter::ValueTypeIDFromMatLibXID(MatLibXID, ValueTypeID);
  assert(succeeded);

  return ParameterValue(ValueTypeID);
}

bool IMaterial::IsParameter(unsigned int ValueTypeID) const
{
  if(Parameter(ValueTypeID)) 
  return true;
  return false;
}

bool IMaterial::Less(const CGraphNode& node) const
{
  const IMaterial* pMat = dynamic_cast<const IMaterial*>(&node);
  if(pMat)
  	return std::lexicographical_compare(m_mpParameters.begin(), m_mpParameters.end(), pMat->m_mpParameters.begin(), pMat->m_mpParameters.end());

  return IMaterialBase::Less(node);
}

// this function is declared pure-virtual because it only adds 'default' parameters
// to the material, but it has to be implemented by a derived class to complete the set
bool IMaterial::Write(const CFFMaterial &ffmat, dia::IDianaRunner& diarunner) const
{
  assert(!Empty());
  assert(dynamic_cast<CGeomecDianaRunnerBase*>(&diarunner));
  CGeomecDianaRunnerBase& runner = static_cast<CGeomecDianaRunnerBase&>(diarunner);

  ftn_double_t ddum;

  ftn_double_t dDecomp[4];

  if(ffmat.IsParameter(IDT_VALUETYPE_GRAINSTIFFNESS))
  {
  ddum = (ftn_double_t) (ffmat.ParameterValue(IDT_VALUETYPE_GRAINSTIFFNESS) * 1e6);
  PutItemLength("BULKS", &ddum, 1); // wjrx mantis 3314
  }

  if(WriteYoungsModulus() && ffmat.IsParameter(IDT_VALUETYPE_YOUNGS_MODULUS))
  {
    ddum = (ftn_double_t) (ffmat.ParameterValue(IDT_VALUETYPE_YOUNGS_MODULUS) * 1e6);
    PutItemLength("YOUNG", &ddum, 1);
  dDecomp[0] = ddum;
  }

  if(WritePoissonRatio() && ffmat.IsParameter(IDT_VALUETYPE_POISSONS_RATIO))
  {
    ddum = (ftn_double_t) (ffmat.ParameterValue(IDT_VALUETYPE_POISSONS_RATIO));
    PutItemLength("POISON", &ddum, 1);
  dDecomp[1] = ddum;
  }

  if(runner.Model().UseDecompactionParameters() &&
     ffmat.IsParameter(IDT_VALUETYPE_YOUNG_DECOMP) &&
     ffmat.IsParameter(IDT_VALUETYPE_POISSON_DECOMP))
  {
  dDecomp[2] = (ftn_double_t)(ffmat.ParameterValue(IDT_VALUETYPE_YOUNG_DECOMP) *1e6);
  dDecomp[3] = (ftn_double_t)(ffmat.ParameterValue(IDT_VALUETYPE_POISSON_DECOMP));
  PutItemLength("DECOMP", dDecomp, 4);
  }

  if(ffmat.IsParameter(IDT_VALUETYPE_RHOB))
  {
    ddum = (ftn_double_t) ffmat.ParameterValue(IDT_VALUETYPE_RHOB);
    PutItem("DENSIT", &ddum);
  }

  if(ffmat.IsParameter(IDT_VALUETYPE_THERMAL_EXPANSION))
  {
    ddum = (ftn_double_t) ffmat.ParameterValue(IDT_VALUETYPE_THERMAL_EXPANSION);
    ddum /= 3.0; // go from volumetric to linear thermal expansion coefficient
    PutItemLength("THERMX", &ddum, 1);
  }

  if(runner.Controller().AnalysisType() == CAnalysisType::AT_MIXTURE ||
     runner.Controller().AnalysisType() == CAnalysisType::AT_MIXTURE_CONTAINMENT)
  {
  // always write the porosity (every material should have it)
  if(!XistIndexed("POROSI", 0))
  {
      ddum = (ftn_double_t) ffmat.ParameterValue(IDT_VALUETYPE_POROSITY);
      PutItem("POROSI", &ddum);
  }

  if(WritePermeability())
  {
      // write modified permeability k' = k / mu
      ftn_double_t mu = (ftn_double_t) ffmat.ParameterValue(IDT_VALUETYPE_VISCOSITY); // in cP (= 1e-3 Pa.s)
      mu *= 1e-3; // Pa.s
      mu /= SECONDS_PER_MONTH; // Pa.month
      ftn_double_t k  = (ftn_double_t) ffmat.ParameterValue(IDT_VALUETYPE_PERMEA); // in mD (= 1e-15 m2)
      k *= 1e-15; // m2
      ftn_double_t k_mod = k / mu;
      PutItemLength("PERMEA", &k_mod, 1);
  }

  // undrained material will write this, so first check
  if(!XistIndexed("BULKF", 0))
  {
      ddum = (ftn_double_t) (ffmat.ParameterValue(IDT_VALUETYPE_FLUID_BULK_MOD) * 1e6);
    PutItem("BULKF", &ddum);
  }

  ddum = (ftn_double_t) ffmat.ParameterValue(IDT_VALUETYPE_FLUIDX);
  PutItem("FLUIDX", &ddum);

  ddum = (ftn_double_t) ffmat.ParameterValue(IDT_VALUETYPE_FLUID_DENSITY);
  PutItem("DENSFL", &ddum);
  }

  if(runner.Controller().AnalysisType() == CAnalysisType::AT_HEAT)
  {
  ddum = (ftn_double_t) ffmat.ParameterValue(IDT_VALUETYPE_THERM_CONDUCT);
  // use months as time unit instead of seconds
  ddum *= SECONDS_PER_MONTH;
  PutItemLength("CONDUC", &ddum, 1);

  ddum = (ftn_double_t) ffmat.ParameterValue(IDT_VALUETYPE_THERM_CAPACI);
  ddum *= (ftn_double_t) ffmat.ParameterValue(IDT_VALUETYPE_RHOB);
  PutItem("CAPACI", &ddum);

  // according to heat flow design document by Chantal Frissen the POROSI
  // material parameter can not be present in heat flow calculation
  if(XistIndexed("POROSI", 0))
      RemoveIndexedItem("POROSI", 0);
  }
  else
  {
  ftn_double_t concex[3];
  concex[0] = 1;
  concex[1] = 1;
  concex[2] = 1;
  PutItemLength("CONCEX", concex, 3);
  }

  return WriteK0(ffmat, diarunner);
}

namespace
{

// we have domain 0,2PI; Diana has -PI/2,PI/2
void fix_k0azimuth_domain(double& azi)
{
  assert(azi >= 0);
  while (azi > M_PI / 2)
  azi -= M_PI;
}

}

bool IMaterial::WriteK0(const CFFMaterial& ffmat, dia::IDianaRunner&) const
{
  assert(!Empty());

  ftn_double_t K0[2];
  K0[0] = (ftn_double_t) ffmat.ParameterValue(IDT_VALUETYPE_LATRATIO_MAX);
  K0[1] = (ftn_double_t) ffmat.ParameterValue(IDT_VALUETYPE_LATRATIO_MIN);
  PutItemLength("K0", K0, 2);

  ftn_double_t azi = (ftn_double_t) DEG2RAD(ffmat.ParameterValue(IDT_VALUETYPE_AZIMUTH));

  fix_k0azimuth_domain(azi);

  PutItem("K0AZIM", &azi);

  return true;
}

int IMaterial::Type() const
{
  return MaterialModel();
}

int IMaterial::WriteFilosParamSize(const CFFMaterial &ffmat, dia::IDianaRunner& diarunner) const
{
  assert(!Empty());
  assert(dynamic_cast<CGeomecDianaRunnerBase*>(&diarunner));
  CGeomecDianaRunnerBase& runner = static_cast<CGeomecDianaRunnerBase&>(diarunner);

  int size = 0;

  if (ffmat.IsParameter(IDT_VALUETYPE_GRAINSTIFFNESS))
  {
  size += 1; // BULKS
  }

  if (WriteYoungsModulus() && ffmat.IsParameter(IDT_VALUETYPE_YOUNGS_MODULUS))
  {
  size += 1; // YOUNG
  }

  if (WritePoissonRatio() && ffmat.IsParameter(IDT_VALUETYPE_POISSONS_RATIO))
  {
  size += 1; // POISON
  }

  if (runner.Model().UseDecompactionParameters() &&
  ffmat.IsParameter(IDT_VALUETYPE_YOUNG_DECOMP) &&
  ffmat.IsParameter(IDT_VALUETYPE_POISSON_DECOMP))
  {
  size += 4; // DECOMP
  }

  if (ffmat.IsParameter(IDT_VALUETYPE_RHOB))
  {
  size += 1; // DENSIT
  }

  if (ffmat.IsParameter(IDT_VALUETYPE_THERMAL_EXPANSION))
  {
  size += 1; // THERMX
  }

  if (runner.Controller().AnalysisType() == CAnalysisType::AT_MIXTURE ||
  runner.Controller().AnalysisType() == CAnalysisType::AT_MIXTURE_CONTAINMENT)
  {
  if (WriteDefaultPorosity())
  {
      size += 1; // POROSI
  }

  if (WritePermeability())
  {
      size += 1; // PERMEA
  }

  if (WriteDefaultFluidBulkModulus())
  {
      size += 1; // BULKF
  }

  size += 2; // FLUIDX/DENSFL
  }

  if (runner.Controller().AnalysisType() == CAnalysisType::AT_HEAT)
  {
  size += 2; // CONDUC/CAPACI
  }
  else
  {
  size += 3; // CONCEX
  }

  size += WriteFilosK0ParamSize(ffmat, diarunner);

  return size;
}

bool IMaterial::WriteFilosParamName(const CFFMaterial &ffmat, dia::IDianaRunner& diarunner, int i, char *name) const
{
  assert(!Empty());
  assert(dynamic_cast<CGeomecDianaRunnerBase*>(&diarunner));
  CGeomecDianaRunnerBase& runner = static_cast<CGeomecDianaRunnerBase&>(diarunner);

  if (i < 0)
  {
  return false;
  }

  if (ffmat.IsParameter(IDT_VALUETYPE_GRAINSTIFFNESS))
  {
  if (i == 0)
  {
      strncpy(name, "BULKS", 10);
      return true;
  }
  --i;
  }

  if (WriteYoungsModulus() && ffmat.IsParameter(IDT_VALUETYPE_YOUNGS_MODULUS))
  {
  if (i == 0)
  {
      strncpy(name, "YOUNG", 10);
      return true;
  }
  --i;
  }

  if (WritePoissonRatio() && ffmat.IsParameter(IDT_VALUETYPE_POISSONS_RATIO))
  {
  if (i == 0)
  {
      strncpy(name, "POISON", 10);
      return true;
  }
  --i;
  }

  if (runner.Model().UseDecompactionParameters() &&
  ffmat.IsParameter(IDT_VALUETYPE_YOUNG_DECOMP) &&
  ffmat.IsParameter(IDT_VALUETYPE_POISSON_DECOMP))
  {
  if (i < 4)
  {
      QString decomp = QString("DECOMP(%1)").arg(i + 1);
      strncpy(name, decomp.toStdString().c_str(), 10);
      return true;
  }

  i -= 4;
  }

  if (ffmat.IsParameter(IDT_VALUETYPE_RHOB))
  {
  if (i == 0)
  {
      strncpy(name, "DENSIT", 10);
      return true;
  }
  --i;
  }

  if (ffmat.IsParameter(IDT_VALUETYPE_THERMAL_EXPANSION))
  {
  if (i == 0)
  {
      strncpy(name, "THERMX", 10);
      return true;
  }
  --i;
  }

  if (runner.Controller().AnalysisType() == CAnalysisType::AT_MIXTURE ||
  runner.Controller().AnalysisType() == CAnalysisType::AT_MIXTURE_CONTAINMENT)
  {
  if (WriteDefaultPorosity())
  {
      if (i == 0)
      {
    strncpy(name, "POROSI", 10);
    return true;
      }
      --i;
  }

  if (WritePermeability())
  {
      if (i == 0)
      {
    strncpy(name, "PERMEA", 10);
    return true;
      }
      --i;
  }

  if (WriteDefaultFluidBulkModulus())
  {
      if (i == 0)
      {
    strncpy(name, "BULKF", 10);
    return true;
      }
      --i;
  }

  if (i == 0)
  {
      strncpy(name, "FLUIDX", 10);
      return true;
  }
  --i;

  if (i == 0)
  {
      strncpy(name, "DENSFL", 10);
      return true;
  }
  --i;
  }

  if (runner.Controller().AnalysisType() == CAnalysisType::AT_HEAT)
  {
  if (i == 0)
  {
      strncpy(name, "CONDUC", 10);
      return true;
  }
  --i;

  if (i == 0)
  {
      strncpy(name, "CAPACI", 10);
      return true;
  }
  --i;
  }
  else
  {
  if (i < 3)
  {
      QString concex = QString("CONCEX(%1)").arg(i + 1);
      strncpy(name, concex.toStdString().c_str(), 10);
      return true;
  }
  i -= 3;
  }

  return WriteFilosK0ParamName(ffmat, diarunner, i, name);
}

void IMaterial::WriteFilosParamValues(const CFFMaterial &ffmat, dia::IDianaRunner& diarunner, double *values, int stride) const
{
  assert(!Empty());
  assert(dynamic_cast<CGeomecDianaRunnerBase*>(&diarunner));
  CGeomecDianaRunnerBase& runner = static_cast<CGeomecDianaRunnerBase&>(diarunner);

  ftn_double_t dDecomp[4];

  if (ffmat.IsParameter(IDT_VALUETYPE_GRAINSTIFFNESS))
  {
  *values = ffmat.ParameterValue(IDT_VALUETYPE_GRAINSTIFFNESS) * 1e6; // BULKS
  values += stride;
  }

  if (WriteYoungsModulus() && ffmat.IsParameter(IDT_VALUETYPE_YOUNGS_MODULUS))
  {
  *values = dDecomp[0] = ffmat.ParameterValue(IDT_VALUETYPE_YOUNGS_MODULUS) * 1e6; // YOUNG
  values += stride;
  }

  if (WritePoissonRatio() && ffmat.IsParameter(IDT_VALUETYPE_POISSONS_RATIO))
  {
  *values = dDecomp[1] = ffmat.ParameterValue(IDT_VALUETYPE_POISSONS_RATIO); // POISON
  values += stride;
  }

  if (runner.Model().UseDecompactionParameters() &&
  ffmat.IsParameter(IDT_VALUETYPE_YOUNG_DECOMP) &&
  ffmat.IsParameter(IDT_VALUETYPE_POISSON_DECOMP))
  {
  *values = dDecomp[0]; // DECOMP(1)
  values += stride;

  *values = dDecomp[1]; // DECOMP(2)
  values += stride;

  *values = ffmat.ParameterValue(IDT_VALUETYPE_YOUNG_DECOMP) *1e6; // DECOMP(3)
  values += stride;

  *values = ffmat.ParameterValue(IDT_VALUETYPE_POISSON_DECOMP); // DECOMP(4)
  values += stride;
  }

  if (ffmat.IsParameter(IDT_VALUETYPE_RHOB))
  {
  *values = ffmat.ParameterValue(IDT_VALUETYPE_RHOB); // DENSIT
  values += stride;
  }

  if (ffmat.IsParameter(IDT_VALUETYPE_THERMAL_EXPANSION))
  {
  *values = ffmat.ParameterValue(IDT_VALUETYPE_THERMAL_EXPANSION) / 3.0; // THERMX
  values += stride;
  }

  if (runner.Controller().AnalysisType() == CAnalysisType::AT_MIXTURE ||
  runner.Controller().AnalysisType() == CAnalysisType::AT_MIXTURE_CONTAINMENT)
  {
  if (WriteDefaultPorosity())
  {
      *values = ffmat.ParameterValue(IDT_VALUETYPE_POROSITY); // POROSI
      values += stride;
  }

  if (WritePermeability())
  {
      // write modified permeability k' = k / mu
      ftn_double_t mu = (ftn_double_t)ffmat.ParameterValue(IDT_VALUETYPE_VISCOSITY); // in cP (= 1e-3 Pa.s)
      mu *= 1e-3; // Pa.s
      mu /= SECONDS_PER_MONTH; // Pa.month
      ftn_double_t k = (ftn_double_t)ffmat.ParameterValue(IDT_VALUETYPE_PERMEA); // in mD (= 1e-15 m2)
      k *= 1e-15; // m2
      ftn_double_t k_mod = k / mu;

      *values = k_mod; // PERMEA
      values += stride;
  }

  // undrained material will write this, so first check
  if (WriteDefaultFluidBulkModulus())
  {
      *values = ffmat.ParameterValue(IDT_VALUETYPE_FLUID_BULK_MOD) * 1e6; // BULKF
      values += stride;
  }

  *values = ffmat.ParameterValue(IDT_VALUETYPE_FLUIDX); // FLUIDX
  values += stride;

  *values = ffmat.ParameterValue(IDT_VALUETYPE_FLUID_DENSITY); // DENSFL
  values += stride;
  }

  if (runner.Controller().AnalysisType() == CAnalysisType::AT_HEAT)
  {
  double ddum = (ftn_double_t)ffmat.ParameterValue(IDT_VALUETYPE_THERM_CONDUCT);
  // use months as time unit instead of seconds
  ddum *= SECONDS_PER_MONTH;

  *values = ddum; // CONDUC
  values += stride;

  ddum = (ftn_double_t)ffmat.ParameterValue(IDT_VALUETYPE_THERM_CAPACI);
  ddum *= (ftn_double_t)ffmat.ParameterValue(IDT_VALUETYPE_RHOB);

  *values = ddum; // CAPACI
  values += stride;
  }
  else
  {
  *values = 1; // CONCEX(1)
  values += stride;
  *values = 1; // CONCEX(2)
  values += stride;
  *values = 1; // CONCEX(3)
  values += stride;
  }

  WriteFilosK0ParamValues(ffmat, diarunner, values, stride);
}


int IMaterial::WriteFilosK0ParamSize(const CFFMaterial &/*ffmat*/, dia::IDianaRunner& /*diarunner*/) const
{
  return 3;
}

bool IMaterial::WriteFilosK0ParamName(const CFFMaterial &/*ffmat*/, dia::IDianaRunner& /*diarunner*/, int i, char *name) const
{
  if (i < 2)
  {
  QString k0 = QString("K0(%1)").arg(i + 1);
  strncpy(name, k0.toStdString().c_str(), 10);
  return true;

  }
  i -= 2;

  if (i == 0)
  {
  strncpy(name, "K0AZIM", 10);
  return true;
  }

  return false;
}

void IMaterial::WriteFilosK0ParamValues(const CFFMaterial &ffmat, dia::IDianaRunner& /*diarunner*/, double *values, int stride) const
{
  *values = ffmat.ParameterValue(IDT_VALUETYPE_LATRATIO_MAX); // K0(1)
  values += stride;

  *values = ffmat.ParameterValue(IDT_VALUETYPE_LATRATIO_MIN); // K0(2)
  values += stride;

  *values = DEG2RAD(ffmat.ParameterValue(IDT_VALUETYPE_AZIMUTH)); // K0AZIM

  fix_k0azimuth_domain(*values);

  values += stride;
}


/*
void IMaterial::Update()
{
  bool bModified = false;

  // Here we first find out which parameter are changed
  int nParams = m_entry.Library().GetMatParamCount(Name().toStdString().c_str());
  assert(nParams == m_mpParameters.size());
  for(int i = 0; i < nParams; i++)
  {
    QString ParamName = m_entry.Library().GetMatParamName(Name().toStdString().c_str(), i);
    CMatParam *Param = m_entry.Library().GetParameter(Name().toStdString().c_str(), ParamName);
    VARIANT va = Param->GetValue();
    assert(va.vt == VT_R8);
    double dValue = va.dblVal;
    unsigned int uiValueTypeID;
    VERIFY(CMaterialParameter::ValueTypeIDFromMatLibXID(std::string(ParamName), uiValueTypeID));
    // Look up parameter
    TParameterMap::iterator it_param = m_mpParameters.find(uiValueTypeID);
    assert(it_param != m_mpParameters.end());
    
    // Is it changed?
    if(it_param->second.Value() != dValue)
    {
      bModified = true;
      it_param->second.Value(dValue);
    }
  }

  if(bModified)
    Modified(MAT_PARAM_CHANGED);
}
*/

bool IMaterial::CanExportToMatLib() const
{
  return m_entry.HasValidMatLib();
}

void IMaterial::ExportToMatLib()
{
  m_entry.ExportToMatLib(LibraryMaterial());
}

bool IMaterial::FixedMaterialModel() const
{
  TServerSet::const_iterator it;
  for(it = Servers().begin(); it != Servers().end(); ++it)
  {
  const CMaterialServer* pMatServer = dynamic_cast<const CMaterialServer*>(*it);

  if(pMatServer && pMatServer->FixedMaterialModel())
      return true;
  }

  return false;
}
void IMaterial::CloneValues(const IMaterial& source_mat)
{
  Name(source_mat.Name());

  m_LibraryMaterial.MaxIterations(source_mat.LibraryMaterial().MaxIterations());
  m_LibraryMaterial.ConvCriterion(source_mat.LibraryMaterial().ConvCriterion());

  size_t i;
  for(i = 0; i < m_LibraryMaterial.ParameterSize(); ++i)
  {
  CLibraryMaterialParameter& targetParam = m_LibraryMaterial.Parameter(i);
  const CLibraryMaterialParameter* pSourceParam = source_mat.LibraryMaterial().Parameter(targetParam.Name());
  if(pSourceParam)
      targetParam.Value(pSourceParam->Value());
  }
}

const CLibraryMaterial& IMaterial::LibraryMaterial() const
{
  return m_LibraryMaterial;
}

CLibraryMaterial& IMaterial::LibraryMaterial()
{
  return m_LibraryMaterial;
}

void IMaterial::LibraryMaterialModified()
{
  if(Name() != m_LibraryMaterial.Name())
  Name(m_LibraryMaterial.Name());

  // rebuild parameters map
  DestroyParameters();
  CreateParameters();

  Modified(MAT_PARAM_CHANGED);

  if(MaterialModel() != m_LibraryMaterial.MaterialModel())
  {
  // this must be replaced
  IMaterial* pMat = CMaterialFactory::getInstance()->Create(m_entry, m_LibraryMaterial);
  pMat->reParent(&m_entry);
  pMat->Name(Name());
  pMat->Index(Index());
  m_entry.OnLibraryMaterialSwitched(m_LibraryMaterial, *pMat);

  // replace material on attached servers
  std::set<CMaterialServer*> stServers = Links<CMaterialServer>();
  std::set<CMaterialServer*>::iterator it;
  for(it = stServers.begin(); it != stServers.end(); ++it)
  {
      CMaterialServer& server = **it;
      CMaterialServer::TValueTypePairVec vcVTypes = server.ValueTypePairVec();
      if(IsLinkedTo(server))
      {
    assert(server.IsLinkedTo(*this));
    server.UnLink(*this);
      }
      server.LinkTo(*pMat);

      // replace value types if possible
      for(size_t i = 0; i < vcVTypes.size(); ++i)
      {
    if(server.CanConnectItem(*vcVTypes[i].first))
          server.ConnectItem(*vcVTypes[i].first);
      }
  }

  // C3PO: "we're doomed..."
  delete this;
  }
}


///// IMaterial::CObserver

IMaterial::CObserver::CObserver(IMaterial& parent, ml::CMaterial& notifier)
: m_parent(parent),
  m_pNotifier(&notifier)
{
  notifier.RegisterObserver(*this);
}

IMaterial::CObserver::~CObserver()
{
  if(m_pNotifier)
  m_pNotifier->UnregisterObserver(*this);
}

void IMaterial::CObserver::Modified()
{
  m_parent.LibraryMaterialModified();
}

void IMaterial::CObserver::ForcedUnregister()
{
  // release reference to notifier
  m_pNotifier = 0;
}
