#ifndef _LoadPropertyBase_h_
#define _LoadPropertyBase_h_

class RGProperty;
class RGInterface;
class CModelBase;
class CFormationBase;
class CDepletionStage;
class CElementValueSet;

namespace GeomecRGI {

class CRockMechProcessor;

} // namespace GeomecRGI

#include "ValueTypeFactory.h"

namespace GeomecRGI {

class CLoadPropertyBase {
public:
  CLoadPropertyBase(const RGProperty &rgProperty, RGInterface &rgi, CModelBase &modelBase, CRockMechProcessor &rmp);
  virtual ~CLoadPropertyBase() = 0;

  bool loadProperty(unsigned int uValueType);
  bool loadProperty(const std::vector<geo::CValue> &vcValues, const QString &strPropertyName, unsigned int uValueType);
  bool loadProperty(const std::vector<std::vector<geo::CValue>> &vcValues, const QString &strPropertyName,
                    unsigned int uValueType);

  virtual bool loadProperty() = 0;

protected:
  virtual unsigned int OverruleValueType(unsigned int uValueType) const;

  virtual void linkValueTypes(CFormationBase &formation, CValueType *pVT, const CDepletionStage &stage,
                              bool fluidPressureFracDetected) const;
  virtual void convertValue(geo::CValue &dValue) const;
  virtual bool addDelta(std::vector<geo::CValue> &vcNodalValues, const QString &strPropertyName,
                        const CFormationBase *pFormation, const geo::IElement &elm, int nNod) const;
  virtual IValueDomainScalar::TValueVec addDeltaSpecific(const CDepletionStage &prevstage,
                                                         const CFormationBase *pFormation, const geo::IElement &elm,
                                                         int nNod) const;

  bool isFractureApertureModel(const CFormationBase &formationBase, const CDepletionStage &depletionStage) const;
  bool isFractureApertureModel(const CFormationBase &formationBase, const CDepletionStage &depletionStage,
                               const geo::IElement &element) const;
  bool addDeltaBase(std::vector<geo::CValue> &vcNodalValues, const QString &strPropertyName,
                    const CFormationBase *pFormation, const geo::IElement &elm, int nNod) const;
  bool isAnisotropic() const;

  virtual bool enforceUniqueness(unsigned int uValueType) const;

  const RGProperty &m_RGProperty;
  RGInterface &m_RGI;
  CModelBase &m_ModelBase;
  CRockMechProcessor &m_RMP;
  const QString m_ValueTypeName;

private:
  CLoadPropertyBase(const CLoadPropertyBase &rhs);
  CLoadPropertyBase &operator=(const CLoadPropertyBase &rhs);

  bool privateLoadProperty(const std::vector<std::vector<geo::CValue>> &vcValues, const QString &strPropertyName,
                           unsigned int uValueType) const;
  bool valueSetIsPresent(const QString &valueTypeName) const;
  bool loadValues(std::vector<geo::CValue> &vcValues, const QString &strPropertyName) const;
  bool assignValues2Elements(CElementValueSet &elementValueSet, bool &fluidPressureFracDetected,
                             const std::vector<std::vector<geo::CValue>> &vcValues, const CDepletionStage &stage,
                             const QString &strPropertyName, unsigned int uValueType, bool firstVolumetricStrain) const;
  void buildValueTypes(CValueType **pVT, CElementValueSet &elementValueSet, unsigned int uValueType) const;
  bool hasFractureApertureFormations() const;
  void linkValueTypes(CValueType *pVT, const CDepletionStage &stage, bool fluidPressureFracDetected) const;
  void addNodalValues(CElementValueSet &elementValueSet, int elementIndex,
                      const std::vector<geo::CValue> &nodalValues) const;

  static void setAnisotropic(bool &isAnisotropic, bool &anisotropicIsSet, RGInterface &rgi, const CModelBase &modelBase,
                             CRockMechProcessor &rmp);

  void unlinkValueTypes(unsigned int uValueType, const CDepletionStage &stage) const;
  void unlinkValueTypes(CFormationBase &formation, unsigned int uValueType, const CDepletionStage &stage) const;

  static bool m_IsAnisotropic;
  static bool m_AnisotropicIsSet;
};

} // namespace GeomecRGI

#endif // _LoadPropertyBase_h_
