
#include <cassert>

#include "MaterialLibraryXML.h"
#include "MaterialHelperFactory.h"
#include "LibraryMaterial.h"
#include "mlxmlFunctions.h"

#include <QtXml/QDomElement>
#include "mlMaterial.h"

///// CMaterialLibraryXML
CMaterialLibraryXML::CMaterialLibraryXML(ml::CMaterialLibrary& matlib)
: mlxml::CMaterialLibraryXML(matlib)
{
}

ml::CMaterial* CMaterialLibraryXML::CreateMaterial(const QString& strName, int iMaterialModel)
{
  const CMaterialHelperFactory *f = CMaterialHelperFactory::Instance();
  assert(f);

  ml::CMaterial::CCreator* pCreator = f->getMatCreator(iMaterialModel);
  if(pCreator)
  {
  ml::CMaterial* pMat = pCreator->OnCreateMaterial();
  pCreator->Create(strName, *pMat);
  return pMat;
  }

  assert(false); // unknown material model
  return 0;
}

mlxml::CMaterialXML* CMaterialLibraryXML::OnCreateMaterialXML(ml::CMaterial &mat)
{
  CLibraryMaterial* pLibMat = dynamic_cast<CLibraryMaterial*>(&mat);
  if(pLibMat)
  return new CMaterialXML(*pLibMat);

  return mlxml::CMaterialLibraryXML::OnCreateMaterialXML(mat);
}

void CMaterialLibraryXML::DestroyMaterial(int iMaterialModel, ml::CMaterial* mat)
{
  const CMaterialHelperFactory *f = CMaterialHelperFactory::Instance();
  assert(f);

  ml::CMaterial::CCreator* pCreator = f->getMatCreator(iMaterialModel);
  if(pCreator)
  {
  pCreator->Destroy(mat);
  }
}

///// CMaterialXML
CMaterialXML::CMaterialXML(CLibraryMaterial& mat)
: mlxml::CMaterialXML(mat)
{
}

void CMaterialXML::Load(QDomElement& domElement)
{
  mlxml::CMaterialXML::Load(domElement);

  // see if the XML contains GEOMEC specific info...
  QDomElement child = domElement.firstChildElement("GeomecMaterial");
  if(!child.isNull())
  {
  CLibraryMaterial& mat = static_cast<CLibraryMaterial&>(Material());
  mat.ConvCriterion(mlxml::AttributeDoubleValue(child, "ConvergenceCriterion"));
  mat.MaxIterations(mlxml::AttributeIntValue(child, "MaximumIterations"));
  }
}

void CMaterialXML::Save(QDomElement& domElement)
{
  mlxml::CMaterialXML::Save(domElement);

  // save GEOMEC specific info
  CLibraryMaterial& mat = static_cast<CLibraryMaterial&>(Material());
  QDomElement child = domElement.ownerDocument().createElement("GeomecMaterial");
  domElement.appendChild(child);
  child.setAttribute("ConvergenceCriterion", mat.ConvCriterion());
  child.setAttribute("MaximumIterations", mat.MaxIterations());
}

mlxml::CMaterialParameterXML* CMaterialXML::OnCreateMaterialParameterXML(ml::CMatParam& matparam)
{
  CLibraryMaterialParameter* pLibMatParam = dynamic_cast<CLibraryMaterialParameter*>(&matparam);
  if(pLibMatParam)
  return new CMaterialParameterXML(*pLibMatParam);

  return mlxml::CMaterialXML::OnCreateMaterialParameterXML(matparam);
}


///// CMaterialParameterXML
CMaterialParameterXML::CMaterialParameterXML(CLibraryMaterialParameter& matparam)
: mlxml::CMaterialParameterXML(matparam)
{
}

void CMaterialParameterXML::Load(QDomElement& domElement)
{
  mlxml::CMaterialParameterXML::Load(domElement);

  CLibraryMaterialParameter& matparam = static_cast<CLibraryMaterialParameter&>(MaterialParameter());
  if(!matparam.CalibrationPath().isEmpty() && !matparam.IsFixedCalibrationParameter())
  {
  // see if the XML contains GEOMEC specific info...
  QDomElement child = domElement.firstChildElement("GeomecMaterialParameter");
  if(!child.isNull())
      matparam.CurrentlyFixed(mlxml::AttributeIntValue(child, "Fixed") != 0);
  }
}

void CMaterialParameterXML::Save(QDomElement& domElement)
{
  mlxml::CMaterialParameterXML::Save(domElement);

  // save GEOMEC specific info
  CLibraryMaterialParameter& matparam = static_cast<CLibraryMaterialParameter&>(MaterialParameter());
  if(!matparam.CalibrationPath().isEmpty() && !matparam.IsFixedCalibrationParameter())
  {
  QDomElement child = domElement.ownerDocument().createElement("GeomecMaterialParameter");
  domElement.appendChild(child);
  child.setAttribute("Fixed", int(matparam.IsCurrentlyFixed() ? 1 : 0));
  }
}
