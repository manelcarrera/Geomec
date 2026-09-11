#ifndef _LoadPropertyMaterialType_h_
#define _LoadPropertyMaterialType_h_

class IMaterial;

#include "LoadPropertyBase.h"

namespace GeomecRGI {

class CLoadPropertyMaterialType : public CLoadPropertyBase {
public:
  CLoadPropertyMaterialType(const RGProperty &rgProperty, RGInterface &rgi, CModelBase &modelBase,
                            CRockMechProcessor &rmp);
  virtual ~CLoadPropertyMaterialType();

  virtual bool loadProperty();

private:
  CLoadPropertyMaterialType(const CLoadPropertyMaterialType &rhs);
  CLoadPropertyMaterialType &operator=(const CLoadPropertyMaterialType &rhs);

  bool mapMaterialType2MaterialModel(std::vector<int> &materialModels, const std::vector<int> &materialTypes) const;
  bool assignMaterialModels() const;
  bool assignMaterialModels(const std::vector<int> &materialModels) const;
  IMaterial *assignMaterialModel(int materialModel) const;
  bool assignMaterialModel() const;

  std::vector<int> m_materialTypes;
};

} // namespace GeomecRGI

#endif // _LoadPropertyMaterialType_h_
