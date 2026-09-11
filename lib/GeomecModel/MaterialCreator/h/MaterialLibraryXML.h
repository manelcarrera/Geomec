#ifndef _MATERIALLIBRARYXML_H_
#define _MATERIALLIBRARYXML_H_

#include "mlxmlMaterial.h"
#include "mlxmlMaterialLibrary.h"
#include "mlxmlMaterialParameter.h"

class CLibraryMaterial;
class CLibraryMaterialParameter;

class CMaterialLibraryXML : public mlxml::CMaterialLibraryXML {
public:
  CMaterialLibraryXML(ml::CMaterialLibrary &matlib);

protected:
  virtual ml::CMaterial *CreateMaterial(const QString &strName, int iMaterialModel);
  virtual mlxml::CMaterialXML *OnCreateMaterialXML(ml::CMaterial &mat);
  virtual void DestroyMaterial(int iMaterialModel, ml::CMaterial *mat);
};

class CMaterialXML : public mlxml::CMaterialXML {
public:
  CMaterialXML(CLibraryMaterial &mat);

  virtual void Load(QDomElement &domElement);
  virtual void Save(QDomElement &domElement);

protected:
  virtual mlxml::CMaterialParameterXML *OnCreateMaterialParameterXML(ml::CMatParam &matparam);
};

class CMaterialParameterXML : public mlxml::CMaterialParameterXML {
public:
  CMaterialParameterXML(CLibraryMaterialParameter &matparam);

  virtual void Load(QDomElement &domElement);
  virtual void Save(QDomElement &domElement);
};

#endif // _MATERIALLIBRARYXML_H_
