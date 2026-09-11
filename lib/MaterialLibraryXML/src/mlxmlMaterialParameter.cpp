#include "mlxmlMaterialParameter.h"

#include "mlMatParam.h"
#include "mlxmlFunctions.h"

#include <QtXml/QDomElement>
#include <QtCore/QObject>
#include <cassert>

namespace mlxml {

/*!
 * \class CMaterialParameterXML
 * \brief Loads and saves an ml::CMatParam object from/to an XML (sub)tree
 */

/*!
 * \brief Constructor
 * \param matparam The material parameter to save or load
 */
CMaterialParameterXML::CMaterialParameterXML(ml::CMatParam& matparam)
: m_matparam(matparam)
{
}

/*!
 * \brief Destructor
 */
CMaterialParameterXML::~CMaterialParameterXML()
{
}

/*!
 * \brief Get the material parameter
 * \return the material parameter this object saves or loads
 * This method can be overridden to supply another object for the
 * Save and Load methods.
 */
ml::CMatParam& CMaterialParameterXML::MaterialParameter()
{
  return m_matparam;
}

/*!
 * \brief Load the material parameter
 * \param domElement The 'MaterialParameter' element to load the data from
 * \throw mlxml::CException on failure
 */
void CMaterialParameterXML::Load(QDomElement& domElement)
{
  assert(AttributeStringValue(domElement, "Name") == m_matparam.Name());
  m_matparam.LoadValue(AttributeDoubleValue(domElement, "Value"));
}

/*!
 * \brief Save the material parameter
 * \param domElement The 'MaterialParameter' element to save the data to
 * \throw mlxml::CException on failure
 */
void CMaterialParameterXML::Save(QDomElement& domElement)
{
  domElement.setAttribute("Name", MaterialParameter().Name());
  domElement.setAttribute("Value", MaterialParameter().Value());
}

} // namespace mlxml
