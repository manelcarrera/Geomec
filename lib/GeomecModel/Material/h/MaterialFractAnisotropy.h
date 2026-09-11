#ifndef _MATERIALFRACTANISOTROPY_H_
#define _MATERIALFRACTANISOTROPY_H_

#include "MaterialFractureApertureBase.h"
#include "MaterialModelFractureAnisotropy.h"
#include "Materials.h"
#include "Matrix.h"

class CMaterialFractAnisotropy : public CMaterialFractureApertureBase
{
public:
  CMaterialFractAnisotropy(CMaterialEntry &entry, CLibraryMaterial& libmat);
  CMaterialFractAnisotropy(const CMaterialFractAnisotropy& material, CMaterialEntry& entry);

  virtual bool Write(const CFFMaterial &ffmat, dia::IDianaRunner& diarunner) const;
  virtual int MaterialModel() const { return MM_FRACTURE_ANISOTROPY; }
  virtual long MaterialModelFilter() const { return MLFO_FRACTURE_ANISOTROPY; }

  // the creator for this material model
  static ml::CMaterial::CCreator* GetLibraryMaterialCreator() { return new CMaterialFractureAnisotropyCreator; }

  // Interface for dia::IElementProperty
  virtual int WriteFilosParamSize(const CFFMaterial &ffmat, dia::IDianaRunner& diarunner) const;
  virtual bool WriteFilosParamName(const CFFMaterial &ffmat, dia::IDianaRunner& diarunner, int i, char *name) const;
  virtual void WriteFilosParamValues(const CFFMaterial &ffmat, dia::IDianaRunner& diarunner, double *values, int stride = 1) const;

  ACCEPT_GEOMECMODELVISITORS(VisitMaterialFractAnisotropy);

private:
  geo::CMatrix CalculateStiffnessMatrix(const CFFMaterial &ffmat,
                    double En,
                    double Et,
                    double Nun,
                    double Nut,
                    double G) const;
  geo::CMatrix CalculateAnisotropyComplianceMatrix(const CFFMaterial &ffmat,
                                                   double En,
                                                   double Et,
                                                   double Nun,
                                                   double Nut,
                                                   double G) const;
  geo::CMatrix CalculateFractureComplianceMatrix(const CFFMaterial &ffmat) const;
  geo::CMatrix RotationMatrix(const geo::CMatrix &base) const;
};

#endif //_MATERIALFRACTANISOTROPY_H_