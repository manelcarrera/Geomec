#ifndef _MATERIALCREATOR_H_
#define _MATERIALCREATOR_H_

#include "mlMatParam.h"
#include "mlMaterial.h"

class CLibraryMaterial;
class IPQPlotter;

class CMaterialCreator : public ml::CMaterial::CCreator {
public:
  virtual ml::CMaterial *OnCreateMaterial();
  virtual void OnCreateParameters(ml::CMaterial &mat);
  virtual IPQPlotter *PQPlotter() const;
  virtual QString CalibrationPath() const = 0;
  virtual bool CanCalibrateElastic() const = 0;
  virtual bool CanCalibratePlastic() const = 0;

protected:
  virtual void OnCreateLibrary(CLibraryMaterial &m) = 0;
  virtual void OnCreateLibraryParameters(CLibraryMaterial &m) = 0;

  void AddNumber(CLibraryMaterial &mat, size_t number);
  void AddLibParam(const QString &strName, double dValue, CLibraryMaterial &parentMat,
                   const ml::CMatParam::CUnitType &unitConversion, unsigned int uiValueTypeID,
                   const ml::CMatParam::CCheckStrategy *pCheckStrategy, const ml::CMatParam::CSetStrategy *pSetStrategy,
                   const QString &strCalibrationPath, bool bFixedCalibrationParam,
                   const QString &strGroupName = QString());
};

#endif // _MATERIALCREATOR_H_
