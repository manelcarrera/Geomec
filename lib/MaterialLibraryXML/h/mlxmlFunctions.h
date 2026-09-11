#ifndef _MLXMLFUNCTIONS_H_
#define _MLXMLFUNCTIONS_H_

class QDomElement;
class QString;

namespace mlxml {

// these functions throw a mlxml::CException when the attribute
// is not found or the value is not of the requested type
double AttributeDoubleValue(QDomElement& domElement, const QString& sName);
int AttributeIntValue(QDomElement& domElement, const QString& sName);
QString AttributeStringValue(QDomElement& domElement, const QString& sName);

} // namespace mlxml

#endif // _MLXMLFUNCTIONS_H_
