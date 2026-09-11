// IMaterialRock.cpp: implementation of the IMaterialRock class.
//
//////////////////////////////////////////////////////////////////////

#include "IMaterialRock.h"
#include "resourceIDI.h"

#include "Material.h"
#include "ValueTypes.h"
#include "lbfl.h"
#include "GeomecDianaRunner.h"

#ifdef _DEBUG
#ifdef _MSC_VER
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#endif  // _MSC_VER
//#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMaterialRock::IMaterialRock(CMaterialEntry &entry, CLibraryMaterial& libmat)
: IMaterial(entry, libmat)
{

}

unsigned int IMaterialRock::IconId() const
{
  if(ReadOnly())
    return IDI_ROCK_MATERIAL_LOCKED;

  if(FixedMaterialModel())
    return IDI_ROCK_MATERIAL_FIXED;

	return IDI_ROCK_MATERIAL;
}

bool IMaterialRock::WriteTransverseIsotropicValues(const CFFMaterial& ffmat, dia::IDianaRunner& diarunner) const
{
  double Et = ffmat.ParameterValue(IDT_VALUETYPE_YOUNGMODULUS_TRANS) * 1e6;
  double En = ffmat.ParameterValue(IDT_VALUETYPE_YOUNGMODULUS_NORM) * 1e6;
  double Nut = ffmat.ParameterValue(IDT_VALUETYPE_POISSONRATIO_TRANS);
  double Nun = ffmat.ParameterValue(IDT_VALUETYPE_POISSONRATIO_NORM);
  double G = ffmat.ParameterValue(IDT_VALUETYPE_ANISOTROPIC_SHEARMODULUS) * 1e6;

  assert(dynamic_cast<CGeomecDianaRunnerBase*>(&diarunner));
  CGeomecDianaRunnerBase& runner = static_cast<CGeomecDianaRunnerBase&>(diarunner);
  if(runner.Model().UseDecompactionParameters())
  {
    // write DECO5 parameter values
    double En_deco = ffmat.ParameterValue(IDT_VALUETYPE_YOUNGMODULUS_NORM_DECOMP) * 1e6;
    double Et_deco = ffmat.ParameterValue(IDT_VALUETYPE_YOUNGMODULUS_TRANS_DECOMP) * 1e6;
    double Nun_deco = ffmat.ParameterValue(IDT_VALUETYPE_POISSONRATIO_NORM_DECOMP);
    double Nut_deco = ffmat.ParameterValue(IDT_VALUETYPE_POISSONRATIO_TRANS_DECOMP);
    double G_deco = ffmat.ParameterValue(IDT_VALUETYPE_ANISOTROPIC_SHEARMODULUS_DECOMP) * 1e6;

    ftn_double_t deco5[10];
    deco5[0] = Et;
    deco5[1] = En;
    deco5[2] = Nut;
    deco5[3] = Nun;
    deco5[4] = G;
    deco5[5] = Et_deco;
    deco5[6] = En_deco;
    deco5[7] = Nut_deco;
    deco5[8] = Nun_deco;
    deco5[9] = G_deco;
    PutItemLength("DECO5", deco5, 10);
  }
  else
  {
    // write linear anisotropic values
    ftn_double_t vals[3];

    vals[0] = Et;
    vals[1] = Et;
    vals[2] = En;
    PutItemLength("YOUNG", vals, 3);

    vals[0] = Nut;
    vals[1] = Nun;
    vals[2] = Nun;
    PutItemLength("POISON", vals, 3);

    vals[0] = Et / (2 * (1 - Nut));
    vals[1] = G;
    vals[2] = G;
    PutItemLength("SHRMOD", vals, 3);
  }

  return true;
}


int IMaterialRock::WriteFilosTransverseIsotropicParamSize(const CFFMaterial &/*ffmat*/, dia::IDianaRunner& diarunner) const
{
  assert(dynamic_cast<CGeomecDianaRunnerBase*>(&diarunner));
  CGeomecDianaRunnerBase& runner = static_cast<CGeomecDianaRunnerBase&>(diarunner);
  if (runner.Model().UseDecompactionParameters())
  {
    return 10;
  }
  else
  {
    return 9;
  }
}

bool IMaterialRock::WriteFilosTransverseIsotropicParamName(const CFFMaterial &/*ffmat*/, dia::IDianaRunner& diarunner, int i, char *name) const
{
  assert(dynamic_cast<CGeomecDianaRunnerBase*>(&diarunner));
  CGeomecDianaRunnerBase& runner = static_cast<CGeomecDianaRunnerBase&>(diarunner);
  if (runner.Model().UseDecompactionParameters())
  {
    if (i < 10)
    {
      QString deco5 = QString("DECO5(%1)").arg(i + 1);
      strncpy(name, deco5.toStdString().c_str(), 10);
      return true;
    }
  }
  else
  {
    if (i < 3)
    {
      QString young = QString("YOUNG(%1)").arg(i + 1);
      strncpy(name, young.toStdString().c_str(), 10);
      return true;
    }
    i -= 3;

    if (i < 3)
    {
      QString poison = QString("POISON(%1)").arg(i + 1);
      strncpy(name, poison.toStdString().c_str(), 10);
      return true;
    }
    i -= 3;

    if (i < 3)
    {
      QString shrmod = QString("SHRMOD(%1)").arg(i + 1);
      strncpy(name, shrmod.toStdString().c_str(), 10);
      return true;
    }
  }

  return false;
}

void IMaterialRock::WriteFilosTransverseIsotropicParamValues(const CFFMaterial &ffmat, dia::IDianaRunner& diarunner, double *&values, int stride) const
{
  double Et = ffmat.ParameterValue(IDT_VALUETYPE_YOUNGMODULUS_TRANS) * 1e6;
  double En = ffmat.ParameterValue(IDT_VALUETYPE_YOUNGMODULUS_NORM) * 1e6;
  double Nut = ffmat.ParameterValue(IDT_VALUETYPE_POISSONRATIO_TRANS);
  double Nun = ffmat.ParameterValue(IDT_VALUETYPE_POISSONRATIO_NORM);
  double G = ffmat.ParameterValue(IDT_VALUETYPE_ANISOTROPIC_SHEARMODULUS) * 1e6;

  assert(dynamic_cast<CGeomecDianaRunnerBase*>(&diarunner));
  CGeomecDianaRunnerBase& runner = static_cast<CGeomecDianaRunnerBase&>(diarunner);
  if (runner.Model().UseDecompactionParameters())
  {
    // write DECO5 parameter values
    double En_deco = ffmat.ParameterValue(IDT_VALUETYPE_YOUNGMODULUS_NORM_DECOMP) * 1e6;
    double Et_deco = ffmat.ParameterValue(IDT_VALUETYPE_YOUNGMODULUS_TRANS_DECOMP) * 1e6;
    double Nun_deco = ffmat.ParameterValue(IDT_VALUETYPE_POISSONRATIO_NORM_DECOMP);
    double Nut_deco = ffmat.ParameterValue(IDT_VALUETYPE_POISSONRATIO_TRANS_DECOMP);
    double G_deco = ffmat.ParameterValue(IDT_VALUETYPE_ANISOTROPIC_SHEARMODULUS_DECOMP) * 1e6;

    *values = Et; // DECO5(1)
    values += stride;

    *values = En; // DECO5(2)
    values += stride;

    *values = Nut; // DECO5(3)
    values += stride;

    *values = Nun; // DECO5(4)
    values += stride;

    *values = G; // DECO5(5)
    values += stride;

    *values = Et_deco; // DECO5(6)
    values += stride;

    *values = En_deco; // DECO5(7)
    values += stride;

    *values = Nut_deco; // DECO5(8)
    values += stride;

    *values = Nun_deco; // DECO5(9)
    values += stride;

    *values = G_deco; // DECO5(10)
    values += stride;

  }
  else
  {
    // write linear anisotropic values

    *values = Et; // YOUNG(1)
    values += stride;

    *values = Et; // YOUNG(2)
    values += stride;

    *values = En; // YOUNG(3)
    values += stride;


    *values = Nut; // POISON(1)
    values += stride;

    *values = Nun; // POISON(2)
    values += stride;

    *values = Nun; // POISON(3)
    values += stride;


    *values = Et / (2 * (1 - Nut)); // SHRMOD(1)
    values += stride;

    *values = G; // SHRMOD(2)
    values += stride;

    *values = G; // SHRMOD(3)
    values += stride;
  }

}