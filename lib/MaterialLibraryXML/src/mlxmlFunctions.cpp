#include "mlxmlFunctions.h"

#include "mlxmlException.h"

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtXml/QDomAttr>
#include <QtXml/QDomElement>

namespace mlxml {

/*!
 * \brief Get a double (floating point) value from an attribute
 * \param domElement XML element containing the attribute
 * \param sName Name of the attribute
 * \return The value as a double
 * \throw mlxml::CException if the attribute cannot be found or the value
 * does not represent a floating point value
 */
double AttributeDoubleValue(QDomElement &domElement, const QString &sName) {
  QDomAttr domAttrName = domElement.attributeNode(sName);
  if (domAttrName.isNull())
    throw CException(QObject::tr("No '%1' attribute found in '%2'").arg(sName).arg(domElement.tagName()));
  bool bOk;
  double dValue = domAttrName.value().toDouble(&bOk);
  if (!bOk)
    throw CException(QObject::tr("Value of attribute '%1' should be floating point").arg(sName));

  return dValue;
}

/*!
 * \brief Get an integer value from an attribute
 * \param domElement XML element containing the attribute
 * \param sName Name of the attribute
 * \return The value as an int
 * \throw mlxml::CException if the attribute cannot be found or the value
 * does not represent an integer value
 */
int AttributeIntValue(QDomElement &domElement, const QString &sName) {
  QDomAttr domAttrName = domElement.attributeNode(sName);
  if (domAttrName.isNull())
    throw CException(QObject::tr("No '%1' attribute found in '%2'").arg(sName).arg(domElement.tagName()));
  bool bOk;
  int iValue = domAttrName.value().toInt(&bOk);
  if (!bOk)
    throw CException(QObject::tr("Value of attribute '%1' should be an integer").arg(sName));

  return iValue;
}

/*!
 * \brief Get a string value from an attribute
 * \param domElement XML element containing the attribute
 * \param sName Name of the attribute
 * \return The value as a string
 * \throw mlxml::CException if the attribute cannot be found
 */
QString AttributeStringValue(QDomElement &domElement, const QString &sName) {
  QDomAttr domAttrName = domElement.attributeNode(sName);
  if (domAttrName.isNull())
    throw CException(QObject::tr("No '%1' attribute found in '%2'").arg(sName).arg(domElement.tagName()));

  return domAttrName.value();
}

} // namespace mlxml