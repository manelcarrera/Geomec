
#include <cassert>

#include "LibraryMaterial.h"
#include "MaterialCreator.h"

#include "MaterialHelperFactory.h"
#include "Materials.h"
#include "ValueTypes.h"

ml::CMaterial *CMaterialCreator::OnCreateMaterial() {
  CLibraryMaterial *pLibMat =
      new CLibraryMaterial(CalibrationPath(), PQPlotter(), CanCalibrateElastic(), CanCalibratePlastic());

  OnCreateLibrary(*pLibMat);

  return pLibMat;
}

void CMaterialCreator::OnCreateParameters(ml::CMaterial &mat) {
  CLibraryMaterial *pLibMat = dynamic_cast<CLibraryMaterial *>(&mat);
  assert(pLibMat);
  if (!pLibMat)
    return;

  OnCreateLibraryParameters(*pLibMat);

  const CMaterialHelperFactory *f = CMaterialHelperFactory::Instance();
  assert(f);

  AddNumber(*pLibMat, 1);
  AddLibParam(MLD_DUMMY, 0, *pLibMat, f->getMPUnitType(CMaterialHelperFactory::UT_NONE), IDT_VALUETYPE_DUMMY, 0, 0,
              QString(), false);
}

IPQPlotter *CMaterialCreator::PQPlotter() const {
  // default returns 0, override to provide implementation
  return 0;
}

void CMaterialCreator::AddNumber(CLibraryMaterial & /*parentMat*/, size_t /*number*/) {}

void CMaterialCreator::AddLibParam(const QString &strName, double dValue, CLibraryMaterial &parentMat,
                                   const ml::CMatParam::CUnitType &unitConversion, unsigned int uiValueTypeID,
                                   const ml::CMatParam::CCheckStrategy *pCheckStrategy,
                                   const ml::CMatParam::CSetStrategy *pSetStrategy, const QString &strCalibrationPath,
                                   bool bFixedCalibrationParam, const QString &strGroupName) {
  Add(new CLibraryMaterialParameter(strName, dValue, parentMat, unitConversion, uiValueTypeID, pCheckStrategy,
                                    pSetStrategy, strCalibrationPath, bFixedCalibrationParam),
      strGroupName);
}