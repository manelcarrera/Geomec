#ifndef _MATERIALFACTORY_H_
#define _MATERIALFACTORY_H_

#include "mlMaterial.h"

class IMaterial;
class CMaterialEntry;
class CLibraryMaterial;

class CMaterialFactory
{
public:
  typedef IMaterial* (*TFactory) (CMaterialEntry&, CLibraryMaterial&);
  typedef ml::CMaterial::CCreator* (*TGetCreator) ();

  static CMaterialFactory* getInstance();

  std::string Register(int iMaterialModel, TFactory createMethod, TGetCreator getCreatorMethod);
  IMaterial* Create(CMaterialEntry& entry, CLibraryMaterial& libmat);
  ml::CMaterial::CCreator* GetLibraryMaterialCreator(int iMaterialModel);

  bool isMaterialRegistered(int materialModel) const;

private:
  CMaterialFactory();
  CMaterialFactory(const CMaterialFactory&);

  template <int MATERIAL_MODEL, class MATERIAL_TYPE>
  void RegisterMaterial();

private:
  typedef std::map<int, std::pair<TFactory, TGetCreator> > TFactoryMap;
  TFactoryMap m_mpFactory;

  TFactoryMap::const_iterator findMaterialModel(int materialModel) const;
};

#endif // _MATERIALFACTORY_H_
