#ifndef _LIBRARYMATERIALPARAMETER_H_
#define _LIBRARYMATERIALPARAMETER_H_

#include "mlMatParam.h"

class CLibraryMaterial;

class CLibraryMaterialParameter : public ml::CMatParam {
public:
  CLibraryMaterialParameter(const QString &strName, double dValue, CLibraryMaterial &parentMat,
                            const CUnitType &unitConversion, unsigned int uiValueTypeID,
                            const CCheckStrategy *pCheckStrategy = 0, const CSetStrategy *pSetStrategy = 0,
                            const QString &strCalibrationPath = QString(), bool bFixedCalibrationParam = false);

  CLibraryMaterialParameter(CLibraryMaterial &parentMat);
  virtual ~CLibraryMaterialParameter();

  virtual CLibraryMaterialParameter *Clone(ml::CMaterial &parentMat) const;

  CLibraryMaterialParameter &operator=(const CLibraryMaterialParameter &rhs);
  bool operator==(const CLibraryMaterialParameter &rhs) const;
  bool operator!=(const CLibraryMaterialParameter &rhs) const;

  const QString &CalibrationPath() const;
  bool IsFixedCalibrationParameter() const;

  bool IsCurrentlyFixed() const;
  void CurrentlyFixed(bool bFixed);

  unsigned int ValueTypeID() const;
  unsigned int IconID() const;

protected:
  CLibraryMaterialParameter(const CLibraryMaterialParameter &rhs, ml::CMaterial &parentMat);

private:
  QString m_strCalibrationPath;
  bool m_bFixedCalibrationParam;
  bool m_bCurrentlyFixed;
  unsigned int m_uiValueTypeID;
};

#endif // _LIBRARYMATERIALPARAMETER_H_
