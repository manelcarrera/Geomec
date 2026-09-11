#ifndef _MLXMLMATERIAL_H_
#define _MLXMLMATERIAL_H_

class QDomElement;

namespace ml {

class CMaterial;
class CMatParam;

} // namespace ml

namespace mlxml {

class CMaterialParameterXML;

class CMaterialXML {
public:
  CMaterialXML(ml::CMaterial &mat);
  virtual ~CMaterialXML();

  virtual ml::CMaterial &Material();

  // these functions throw an mlxml::CException on failure
  virtual void Load(QDomElement &domElement);
  virtual void Save(QDomElement &domElement);

protected:
  virtual CMaterialParameterXML *OnCreateMaterialParameterXML(ml::CMatParam &matparam);

private:
  ml::CMaterial &m_mat;
};

} // namespace mlxml

#endif // _MLXMLMATERIAL_H_
