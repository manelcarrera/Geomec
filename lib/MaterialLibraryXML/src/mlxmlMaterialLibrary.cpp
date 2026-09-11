#include "mlxmlMaterialLibrary.h"
#include "mlMaterialLibrary.h"
#include "mlxmlMaterial.h"
#include "mlxmlFunctions.h"
#include "mlxmlException.h"

#include <QtXml/QDomElement>
#include <QtCore/QFile>
#include <QtCore/QTextStream>
#include <vector>

/*!
 * The namespace containing all classes that handle loading and
 * saving of ml::CMaterialLibrary, ml::CMaterial and ml::CMatParam
 * objects to an XML format.
 * These classes can be used to import and export a material library
 * or specific materials in it.
 */
namespace mlxml {

/*!
 * \class CMaterialLibraryXML
 * \brief Loads and saves a ml::CMaterialLibrary object to an XML (sub)tree
 * This class can be used to load or save the material library from/to a
 * file (see LoadLibrary and SaveLibrary) or from/to an XML subtree directly
 * (see Load and Save). The Load and Save methods are used by the
 * LoadLibrary and SaveLibrary method and can be overridden to reimplement
 * or extend the functionality.
 */

/*!
 * \brief Constructor
 * \param matlib The material library to save or load
 */
CMaterialLibraryXML::CMaterialLibraryXML(ml::CMaterialLibrary& matlib)
: m_matlib(matlib)
{
}

/*!
 * \brief Destructor
 */
CMaterialLibraryXML::~CMaterialLibraryXML()
{
}

/*!
 * \brief Get the material library
 * \return the material library this object saves or loads
 * This method can be overridden to supply another object for the
 * Save and Load methods.
 */
ml::CMaterialLibrary& CMaterialLibraryXML::MaterialLibrary()
{
  return m_matlib;
}

/*!
 * \brief Load the material library from a file
 * \param sFileName The path to the file containing the XML
 * This method loads the material library from a file. It calls
 * the Load method to do the actual loading. The root element in
 * the XML file must be "MaterialLibrary".
 * \note The loading functions do not clear the provided material
 * library, they only append the data from the file.
 * \throw mlxml::CException on failure
 */
void CMaterialLibraryXML::LoadFile(const QString& sFileName)
{
  QFile file(sFileName);
  if(!file.open(QIODevice::ReadOnly))
    throw CException(QObject::tr("Unable to open file '%1' for reading").arg(sFileName));

  QDomDocument doc;
  if(!doc.setContent(&file))
  {
    file.close();
    throw CException(QObject::tr("Unable to parse the content of file '%1'").arg(sFileName));
  }

  QDomElement docElement = doc.documentElement();
  if(docElement.tagName() != "MaterialLibrary")
    throw CException(QObject::tr("The root element of the XML file must be 'MaterialLibrary'"));

  Load(docElement);
}

/*!
 * \brief Save the material library to a file
 * \param sFileName The path to the file that will be created.
 * This method saves the material library to a file. The file will
 * be created. If the file exists it will be overwritten.
 * SaveLibrary calls the Save method to do the actual saving. The
 * root element in the XML file will be "MaterialLibrary".
 * \throw mlxml::CException on failure
 */
void CMaterialLibraryXML::SaveFile(const QString& sFileName)
{
  QFile file(sFileName);
  if(!file.open(QIODevice::WriteOnly | QIODevice::Text))
    throw CException(QObject::tr("Unable to open file '%1' for writing").arg(sFileName));

  QDomDocument doc("MaterialLibrary");
  QDomElement root = doc.createElement("MaterialLibrary");
  doc.appendChild(root);

  Save(root);

  QTextStream stream(&file);
  stream << doc.toString();
}

/*!
 * \brief Load the material library
 * \param domElement The 'MaterialLibrary' element to load the data from
 * \throw mlxml::CException on failure
 */
void CMaterialLibraryXML::Load(QDomElement& domElement)
{
  std::vector<std::pair<int, ml::CMaterial*> > vcLoadedMaterials;

  try
  {
    QDomElement child = domElement.firstChildElement("Material");
    while(!child.isNull())
    {
      QString strName = AttributeStringValue(child, "Name");
      int iMaterialModel = AttributeIntValue(child, "MaterialModel");
      ml::CMaterial* pMaterial = CreateMaterial(strName, iMaterialModel);
      if(pMaterial)
      {
        CMaterialXML* pMatxml = OnCreateMaterialXML(*pMaterial);
        pMatxml->Load(child);
        delete pMatxml;

        vcLoadedMaterials.push_back(std::make_pair(iMaterialModel, pMaterial));
      }

      child = child.nextSiblingElement("Material");
    }
  }
  catch(CException e)
  {
    // delete any created materials
    size_t i;
    for(i = 0; i < vcLoadedMaterials.size(); ++i)
      DestroyMaterial(vcLoadedMaterials[i].first, vcLoadedMaterials[i].second);

    throw;
  }

  // add loaded materials to library
  size_t i;
  for(i = 0; i < vcLoadedMaterials.size(); ++i)
    MaterialLibrary().AddMaterial(*vcLoadedMaterials[i].second);
}

/*!
 * \brief Save the material library
 * \param domElement The 'MaterialLibrary' element to save the data to
 * \throw mlxml::CException on failure
 */
void CMaterialLibraryXML::Save(QDomElement& domElement)
{
  int i;
  for(i = 0; i < MaterialLibrary().MaterialSize(); ++i)
  {
    QDomElement child = domElement.ownerDocument().createElement("Material");
    domElement.appendChild(child);
    CMaterialXML* pMatxml = OnCreateMaterialXML(MaterialLibrary().Material(i));
    pMatxml->Save(child);
    delete pMatxml;
  }
}

/*!
 * \brief Create a CMaterialXML instance
 * \param mat ml::CMaterial object that will be saved or loaded
 * \return the CMaterialXML instance that can save or load the given material
 * Derived classes should override this function to create an instance of a
 * derived class of CMaterialXML.
 */
CMaterialXML* CMaterialLibraryXML::OnCreateMaterialXML(ml::CMaterial& mat)
{
  return new CMaterialXML(mat);
}

/*!
 * \function virtual ml::CMaterial* CreateMaterial(const QString& strName, int iMaterialModel)
 * \brief Get a material given its name and material model
 * \return a pointer to a material with the given name and material model, or NULL if such
 * a material cannot be created
 * This method must be implemented in an inherited class to provide material objects given
 * a name and a material model identifier. If an unknown material model identifier is provided
 * return NULL, the material will be ignored in that case.
 */

} // namespace mlxml
