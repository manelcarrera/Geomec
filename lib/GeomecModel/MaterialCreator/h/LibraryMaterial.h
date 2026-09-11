#ifndef _LIBRARYMATERIAL_H_
#define _LIBRARYMATERIAL_H_

#include "mlMaterial.h"

#include "LibraryMaterialParameter.h"
#include "StressStrain.h"

const int NUM_PQ_STEPS = 50;

class CMaterialCreator;
class CLibraryMaterial;

#ifdef TEST_MATERIALS
namespace test_materials
{
  class AccessLibMaterial;
}
#endif

class IPQPlotter
{
public:
  virtual ~IPQPlotter() {}
  virtual IPQPlotter* Clone() const = 0;
  virtual void GetPrimaryYieldCurve(const CLibraryMaterial& mat, double dPMin, double dPMax, CStressStrainArray& values) const = 0;
  virtual void GetSecondaryYieldCurve(const CLibraryMaterial& /*mat*/, double /*dPMin*/, double /*dPMax*/, CStressStrainArray& /*values*/) const {}
};

class CLibraryMaterial : public ml::CMaterial
{
protected:
  friend class CMaterialCreator;

  CLibraryMaterial(const QString& strCalibrationPath, IPQPlotter* pPQPlotter, bool bCanCalibrateLinear, bool bCanCalibrateNonlinear);
  virtual ~CLibraryMaterial();

public:
  virtual CLibraryMaterial* Clone() const;

  CLibraryMaterial& operator=(const CLibraryMaterial& rhs);
  bool operator==(const CLibraryMaterial& rhs) const;
  bool operator!=(const CLibraryMaterial& rhs) const;

  int MaxIterations() const;
  double ConvCriterion() const;

  void MaxIterations(int nMaxIterations);
  void ConvCriterion(double dConvCriterion);

  virtual size_t ParameterSize() const;
  // by index, superclass
  virtual const ml::CMatParam& MatParameter(size_t i) const;
  virtual ml::CMatParam& MatParameter(size_t i);

  // by index
  virtual const CLibraryMaterialParameter& Parameter(size_t i) const;
  virtual CLibraryMaterialParameter& Parameter(size_t i);

  // by name
  virtual const CLibraryMaterialParameter* Parameter(const QString& strName) const;
  virtual CLibraryMaterialParameter* Parameter(const QString& strName);

  // by value type ID
  const CLibraryMaterialParameter* ParameterByValueTypeID(unsigned int uiValueTypeID) const;
  CLibraryMaterialParameter* ParameterByValueTypeID(unsigned int uiValueTypeID);

  bool GetPrimaryYieldCurve(double dPMin, double dPMax, CStressStrainArray& values) const;
  bool GetSecondaryYieldCurve(double dPMin, double dPMax, CStressStrainArray& values) const;

  bool CanCalibrateElastic() const;
  bool CanCalibratePlastic() const;

  virtual void SwitchMaterialModel(CCreator& creator, bool bKeepParamValues);

  int ReadFromFilos();
  bool WriteToFilos() const;

  bool BehavesLinearly() const;
  void BehavesLinearly(bool bBehavesLinearly);

protected:
  CLibraryMaterial(const CLibraryMaterial& rhs);

  virtual void CopyParameters(const ml::CMaterial& rhs);
  virtual size_t AddParameter(ml::CMatParam *param);
  virtual void DestroyParameters();

private:
  QString m_strCalibrationPath;
  bool m_bCanCalibrateElastic;
  bool m_bCanCalibratePlastic;
  bool m_bBehavesLinearly;
#ifdef TEST_MATERIALS
  friend class test_materials::AccessLibMaterial;
#endif
  IPQPlotter* m_pPQPlotter;
  int m_nMaxIterations;
  double m_dConvCriterion;

  std::vector<CLibraryMaterialParameter *> m_vcParameters;
};

#endif // _LIBRARYMATERIAL_H_
