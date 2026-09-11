#ifndef _MATERIALFRACTUREAPERTUREBASE_H_
#define _MATERIALFRACTUREAPERTUREBASE_H_

#include "IMaterialRock.h"
#include "Vector.h"
#include "Materials.h"

class CMaterialFractureApertureBase : public IMaterialRock
{
public:
  CMaterialFractureApertureBase(CMaterialEntry& entry, CLibraryMaterial& libmat);

  virtual bool Write(const CFFMaterial& ffmat, dia::IDianaRunner& diarunner) const;

  // direction vector for the highest and lowers fracture density
  // these vector are normalized
  geo::CVector HighDensityDirection(const CFFMaterial& ffmat) const;
  geo::CVector IntermediateDensityDirection(const CFFMaterial& ffmat) const;
  geo::CVector LowDensityDirection(const CFFMaterial& ffmat) const;

  // Interface for dia::IElementProperty
  virtual int WriteFilosParamSize(const CFFMaterial &ffmat, dia::IDianaRunner& diarunner) const;
  virtual bool WriteFilosParamName(const CFFMaterial &ffmat, dia::IDianaRunner& diarunner, int i, char *name) const;
  virtual void WriteFilosParamValues(const CFFMaterial &ffmat, dia::IDianaRunner& diarunner, double *values, int stride = 1) const;

  ACCEPT_GEOMECMODELVISITORS(VisitMaterialFractureApertureBase);

protected:
  virtual bool WriteYoungsModulus() const { return false; }
  virtual bool WritePoissonRatio() const { return false; }

private:
  geo::CVector VectorFromAngles(double dInclination, double dAzimuth) const;
};

#endif // _MATERIALFRACTUREAPERTURE_H_
