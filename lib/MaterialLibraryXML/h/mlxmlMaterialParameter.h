#ifndef _MLXMLMATERIALPARAMETER_H_
#define _MLXMLMATERIALPARAMETER_H_

class QDomElement;

namespace ml {

class CMatParam;

} // namespace ml

namespace mlxml {

class CMaterialParameterXML
{
public:
  CMaterialParameterXML(ml::CMatParam& matparam);
  virtual ~CMaterialParameterXML();

  virtual ml::CMatParam& MaterialParameter();

  // these functions throw an mlxml::CException on failure
  virtual void Load(QDomElement& domElement);
  virtual void Save(QDomElement& domElement);

private:
  ml::CMatParam& m_matparam;
};

} // namespace mlxml

#endif // _MLXMLMATERIALPARAMETER_H_
