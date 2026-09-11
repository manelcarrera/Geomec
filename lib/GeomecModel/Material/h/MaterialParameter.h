#ifndef _MATERIALPARAMETER_H_
#define _MATERIALPARAMETER_H_

#include <map>

class CLibraryMaterialParameter;
class QString;

class CMaterialParameter
{
public:
  CMaterialParameter(unsigned int uiValueTypeID, CLibraryMaterialParameter& LibraryParameter);
  ~CMaterialParameter();

  bool operator<(const CMaterialParameter& rhs) const;

  double Value() const;
//	void Value(const double& dValue);
  unsigned int ValueTypeID() const;
//	CString MatLibXString() const;

  // load is done by the parent material
  long SavedItems() const;

  static bool ValueTypeIDFromMatLibXID(const QString& MatLibXID, unsigned int& ValueTypeID);
  static bool MatLibXIDFromValueTypeID(unsigned int ValueTypeID, QString& MatLibXID);

private:
  CLibraryMaterialParameter& m_LibraryParameter;
//	double m_dValue;		// value of this parameter
  unsigned int m_uiValueTypeID;	// value type ID from GEOMEC
//	CString m_strMatLibXID;	// parameter name (unique ID) from MatLibX control

  static std::map<unsigned int, QString> m_Vt2Str; // get matlibx string id from value type id
  static std::map<QString, unsigned int> m_Str2Vt; // get value type id from matlibx string id

  static void BuildValueTypeMaps();
};


#endif /* _MATERIALPARAMETER_H_ */