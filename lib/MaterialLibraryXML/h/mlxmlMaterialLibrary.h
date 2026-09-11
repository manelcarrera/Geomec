#ifndef _MLXMLMATERIALLIBRARY_H_
#define _MLXMLMATERIALLIBRARY_H_

class QDomElement;
class QString;

namespace ml {

class CMaterialLibrary;
class CMaterial;

}

namespace mlxml {

class CMaterialXML;

class CMaterialLibraryXML
{
public:
  CMaterialLibraryXML(ml::CMaterialLibrary& matlib);
  virtual ~CMaterialLibraryXML();

  virtual ml::CMaterialLibrary& MaterialLibrary();

  // these functions throw an mlxml::CException on failure
  void LoadFile(const QString& sFileName);
  void SaveFile(const QString& sFileName);

  // these functions throw an mlxml::CException on failure
  virtual void Load(QDomElement& domElement);
  virtual void Save(QDomElement& domElement);

protected:
  virtual CMaterialXML* OnCreateMaterialXML(ml::CMaterial& mat);
  virtual ml::CMaterial* CreateMaterial(const QString& strName, int iMaterialModel) = 0;
  virtual void DestroyMaterial(int iMaterialModel, ml::CMaterial* mat) = 0;

private:
  ml::CMaterialLibrary& m_matlib;
};

} // namespace mlxml

#endif // _MLXMLMATERIALLIBRARY_H_
