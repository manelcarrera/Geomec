#include "mlxmlMaterial.h"
#include "mlxmlFunctions.h"
#include "mlMaterial.h"
#include "mlxmlMaterialParameter.h"
#include "mlMatParam.h"

#include <QtXml/QDomElement>

namespace mlxml {

/*!
 * \class CMaterialXML
 * \brief Loads and saves an ml::CMaterial object from/to an XML (sub)tree
 */

/*!
 * \brief Constructor
 * \param mat The material to save or load
 */
CMaterialXML::CMaterialXML(ml::CMaterial& mat)
: m_mat(mat)
{
}

/*!
 * \brief Destructor
 */
CMaterialXML::~CMaterialXML()
{
}

/*!
 * \brief Get the material
 * \return the material this object saves or loads
 * This method can be overridden to supply another object for the
 * Save and Load methods.
 */
ml::CMaterial& CMaterialXML::Material()
{
  return m_mat;
}

/*!
 * \brief Load the material
 * \param domElement The 'Material' element to load the data from
 * \throw mlxml::CException on failure
 */
void CMaterialXML::Load(QDomElement& domElement)
{
  QDomElement child = domElement.firstChildElement("MaterialParameter");
  while(!child.isNull())
  {
    QString sName = AttributeStringValue(child, "Name");
    size_t i;
    for(i = 0; i < Material().ParameterSize(); ++i)
    {
      ml::CMatParam& matparam = Material().MatParameter(i);
      if(matparam.Name() == sName)
      {
        CMaterialParameterXML* pMatParamxml = OnCreateMaterialParameterXML(matparam);
        pMatParamxml->Load(child);
        delete pMatParamxml;
      }
    }

    child = child.nextSiblingElement("MaterialParameter");
  }
}

/*!
 * \brief Save the material
 * \param domElement The 'Material' element to save the data to
 * \throw mlxml::CException on failure
 */
void CMaterialXML::Save(QDomElement& domElement)
{
  domElement.setAttribute("Name", Material().Name());
  domElement.setAttribute("MaterialModel", Material().MaterialModel());

  // save parameters
  size_t i;
  for(i = 0; i < Material().ParameterSize(); ++i)
  {
    QDomElement child = domElement.ownerDocument().createElement("MaterialParameter");
    domElement.appendChild(child);
    CMaterialParameterXML* pMatParamxml = OnCreateMaterialParameterXML(Material().MatParameter(i));
    pMatParamxml->Save(child);
    delete pMatParamxml;
  }
}

/*!
 * \brief Create a CMaterialParameterXML instance
 * \param mat ml::CMatParam object that will be saved or loaded
 * \return the CMaterialParameterXML instance that can save or load the given material
 * Derived classes should override this function to create an instance of a
 * derived class of CMaterialParameterXML.
 */
CMaterialParameterXML* CMaterialXML::OnCreateMaterialParameterXML(ml::CMatParam &matparam)
{
  return new CMaterialParameterXML(matparam);
}

} // namespace mlxml
